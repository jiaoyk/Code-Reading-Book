// client.cpp,v 1.13 2001/10/04 03:47:34 irfan Exp

#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Stats.h"
#include "ace/Sample_History.h"
#include "ace/Read_Buffer.h"
#include "ace/Array_Base.h"
#include "ace/Task.h"
#include "tao/ORB_Core.h"
#include "tao/RTCORBA/RTCORBA.h"
#include "tao/RTCORBA/Priority_Mapping_Manager.h"
#include "testC.h"
#include "tests/RTCORBA/Linear_Priority/readers.cpp"
#include "fudge_priorities.cpp"

ACE_RCSID(Thread_Pool, client, "client.cpp,v 1.13 2001/10/04 03:47:34 irfan Exp")

enum Priority_Setting
{
  AT_THREAD_CREATION = 0,
  AFTER_THREAD_CREATION = 1
};

static const char *ior = "file://ior";
static const char *rates_file = "rates";
static const char *invocation_priorities_file = "empty-file";
static int shutdown_server = 0;
static int do_dump_history = 0;
static ACE_UINT32 gsf = 0;
static CORBA::ULong continuous_workers = 0;
static int done = 0;
static CORBA::ULong time_for_test = 10;
static CORBA::ULong work = 10;
static CORBA::ULong max_throughput_timeout = 5;
static CORBA::ULong continuous_worker_priority = 0;
static int set_priority = 1;
static Priority_Setting priority_setting = AFTER_THREAD_CREATION;
static int individual_continuous_worker_stats = 0;
static int print_missed_invocations = 0;
static int continuous_workers_are_rt = 1;
static ACE_hrtime_t test_start;
static ACE_Time_Value start_of_test;

struct Synchronizers
{
  Synchronizers (void)
    : worker_lock_ (),
      workers_ (1),
      workers_ready_ (0),
      number_of_workers_ (0)
  {
  }

  ACE_SYNCH_MUTEX worker_lock_;
  ACE_Event workers_;
  CORBA::ULong workers_ready_;
  CORBA::ULong number_of_workers_;
};

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "c:h:i:k:m:p:r:s:t:u:v:w:x:y:z:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'c':
        continuous_workers =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'h':
        do_dump_history =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'i':
        individual_continuous_worker_stats =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'k':
        ior =
          get_opts.optarg;
        break;

      case 'm':
        print_missed_invocations =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'p':
        invocation_priorities_file =
          get_opts.optarg;
        break;

      case 'r':
        rates_file =
          get_opts.optarg;
        break;

      case 's':
        continuous_workers_are_rt =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 't':
        time_for_test =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'u':
        continuous_worker_priority =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'v':
        priority_setting =
          Priority_Setting (ACE_OS::atoi (get_opts.optarg));
        break;

      case 'w':
        work =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'x':
        shutdown_server =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'y':
        set_priority =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case 'z':
        max_throughput_timeout =
          ACE_OS::atoi (get_opts.optarg);
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s\n"
                           "\t-c <number of continuous workers> (defaults to %d)\n"
                           "\t-h <show history> (defaults to %d)\n"
                           "\t-i <print stats of individual continuous workers> (defaults to %d)\n"
                           "\t-k <ior> (defaults to %s)\n"
                           "\t-m <print missed invocations for paced workers> (defaults to %d)\n"
                           "\t-p <invocation priorities file> (defaults to %s)\n"
                           "\t-r <rates file> (defaults to %s)\n"
                           "\t-s <continuous workers have real time scope and scheduling policies> (defaults to %d)\n"
                           "\t-t <time for test> (defaults to %d)\n"
                           "\t-u <continuous worker priority> (defaults to %d)\n"
                           "\t-v <priority setting: AT_THREAD_CREATION = 0, AFTER_THREAD_CREATION = 1> (defaults to %s)\n"
                           "\t-w <work> (defaults to %d)\n"
                           "\t-x <shutdown server> (defaults to %d)\n"
                           "\t-y <set invocation priorities> (defaults to %d)\n"
                           "\t-z <timeout for max throughput measurement> (defaults to %d)\n"
                           "\n",
                           argv [0],
                           continuous_workers,
                           do_dump_history,
                           individual_continuous_worker_stats,
                           ior,
                           print_missed_invocations,
                           invocation_priorities_file,
                           rates_file,
                           continuous_workers_are_rt,
                           time_for_test,
                           continuous_worker_priority,
                           priority_setting == 0 ? "AT_THREAD_CREATION" : "AFTER_THREAD_CREATION",
                           work,
                           shutdown_server,
                           set_priority,
                           max_throughput_timeout),
                          -1);
      }

  return 0;
}

int
start_synchronization (test_ptr test,
                       Synchronizers &synchronizers)
{
  ACE_TRY_NEW_ENV
    {
      test->method (work,
                    ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return -1;
    }
  ACE_ENDTRY;

  {
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX,
                      mon,
                      synchronizers.worker_lock_,
                      -1);

    if (synchronizers.workers_ready_ == 0)
      {
        if (TAO_debug_level > 0)
          ACE_DEBUG ((LM_DEBUG,
                      "\n"));
      }

    ++synchronizers.workers_ready_;

    if (TAO_debug_level > 0)
      ACE_DEBUG ((LM_DEBUG,
                  "%d worker ready\n",
                  synchronizers.workers_ready_));

    if (synchronizers.workers_ready_ ==
        synchronizers.number_of_workers_)
      {
        if (TAO_debug_level > 0)
          ACE_DEBUG ((LM_DEBUG,
                      "\n"));

        start_of_test =
          ACE_OS::gettimeofday ();

        test_start =
          ACE_OS::gethrtime ();

        synchronizers.workers_.signal ();

        return 0;
      }
  }

  synchronizers.workers_.wait ();

  return 0;
}

int
end_synchronization (Synchronizers &synchronizers)
{
  {
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX,
                      mon,
                      synchronizers.worker_lock_,
                      -1);

    if (synchronizers.workers_ready_ ==
        synchronizers.number_of_workers_)
      {
        if (TAO_debug_level > 0)
          ACE_DEBUG ((LM_DEBUG,
                      "\n"));

        synchronizers.workers_.reset ();
      }

    if (TAO_debug_level > 0)
      ACE_DEBUG ((LM_DEBUG,
                  "%d worker completed\n",
                  synchronizers.workers_ready_));

    --synchronizers.workers_ready_;

    if (synchronizers.workers_ready_ == 0)
      {
        if (TAO_debug_level > 0)
          ACE_DEBUG ((LM_DEBUG,
                      "\n"));

        synchronizers.workers_.signal ();

        return 0;
      }
  }

  synchronizers.workers_.wait ();

  return 0;
}

int
max_throughput (test_ptr test,
                RTCORBA::Current_ptr current,
                RTCORBA::PriorityMapping &priority_mapping,
                CORBA::ULong &max_rate)
{
  CORBA::ULong calls_made = 0;
  CORBA::Short CORBA_priority = 0;
  CORBA::Short native_priority = 0;

  ACE_TRY_NEW_ENV
    {
      CORBA_priority =
        current->the_priority (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      CORBA::Boolean result =
        priority_mapping.to_native (CORBA_priority,
                                    native_priority);
      if (!result)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Error in converting CORBA priority %d to native priority\n",
                           CORBA_priority),
                          -1);

      ACE_Time_Value start =
        ACE_OS::gettimeofday ();

      ACE_Time_Value end =
        start + ACE_Time_Value (max_throughput_timeout);

      for (;;)
        {
          ACE_Time_Value now =
            ACE_OS::gettimeofday ();

          if (now > end)
            break;

          test->method (work,
                        ACE_TRY_ENV);
          ACE_TRY_CHECK;

          ++calls_made;
        }
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return -1;
    }
  ACE_ENDTRY;

  max_rate =
    calls_made / max_throughput_timeout;

  ACE_DEBUG ((LM_DEBUG,
              "\nPriority = %d/%d; Max rate calculations => %d calls in %d seconds; Max rate = %d\n",
              CORBA_priority,
              native_priority,
              calls_made,
              max_throughput_timeout,
              max_rate));

  return 0;
}

class Paced_Worker :
  public ACE_Task_Base
{
public:
  Paced_Worker (ACE_Thread_Manager &thread_manager,
                test_ptr test,
                CORBA::Short rate,
                CORBA::ULong iterations,
                CORBA::Short priority,
                RTCORBA::Current_ptr current,
                RTCORBA::PriorityMapping &priority_mapping,
                Synchronizers &synchronizers);

  int svc (void);
  ACE_Time_Value deadline_for_current_call (CORBA::ULong i);
  void reset_priority (CORBA::Environment &ACE_TRY_ENV);
  void print_stats (ACE_hrtime_t test_end);
  int setup (CORBA::Environment &ACE_TRY_ENV);

  test_var test_;
  int rate_;
  ACE_Sample_History history_;
  CORBA::Short priority_;
  RTCORBA::Current_var current_;
  RTCORBA::PriorityMapping &priority_mapping_;
  Synchronizers &synchronizers_;
  CORBA::Short CORBA_priority_;
  CORBA::Short native_priority_;
  ACE_Time_Value interval_between_calls_;
  CORBA::ULong deadlines_missed_;

  typedef ACE_Array_Base<CORBA::ULong> Missed_Invocations;
  Missed_Invocations missed_invocations_;
};

Paced_Worker::Paced_Worker (ACE_Thread_Manager &thread_manager,
                            test_ptr test,
                            CORBA::Short rate,
                            CORBA::ULong iterations,
                            CORBA::Short priority,
                            RTCORBA::Current_ptr current,
                            RTCORBA::PriorityMapping &priority_mapping,
                            Synchronizers &synchronizers)
  : ACE_Task_Base (&thread_manager),
    test_ (test::_duplicate (test)),
    rate_ (rate),
    history_ (iterations),
    priority_ (priority),
    current_ (RTCORBA::Current::_duplicate (current)),
    priority_mapping_ (priority_mapping),
    synchronizers_ (synchronizers),
    CORBA_priority_ (0),
    native_priority_ (0),
    interval_between_calls_ (),
    deadlines_missed_ (0),
    missed_invocations_ (iterations)
{
  this->interval_between_calls_.set (1 / double (this->rate_));
}

void
Paced_Worker::reset_priority (CORBA::Environment &ACE_TRY_ENV)
{
  if (set_priority)
    {
      this->current_->the_priority (this->priority_,
                                    ACE_TRY_ENV);
      ACE_CHECK;
    }
  else
    {
      this->current_->the_priority (continuous_worker_priority,
                                    ACE_TRY_ENV);
      ACE_CHECK;
    }
}

ACE_Time_Value
Paced_Worker::deadline_for_current_call (CORBA::ULong i)
{
  ACE_Time_Value deadline_for_current_call =
    this->interval_between_calls_;

  deadline_for_current_call *= i;

  deadline_for_current_call += start_of_test;

  return deadline_for_current_call;
}

void
Paced_Worker::print_stats (ACE_hrtime_t test_end)
{
  ACE_GUARD (ACE_SYNCH_MUTEX,
             mon,
             this->synchronizers_.worker_lock_);

  ACE_DEBUG ((LM_DEBUG,
              "\n************ Statistics for thread %t ************\n\n"));

  ACE_DEBUG ((LM_DEBUG,
              "Priority = %d/%d; Rate = %d/sec; Iterations = %d; "
              "deadlines made = %d; deadlines missed = %d; Success = %d%%\n",
              this->CORBA_priority_,
              this->native_priority_,
              this->rate_,
              this->history_.max_samples (),
              this->history_.sample_count (),
              this->deadlines_missed_,
              this->history_.sample_count () * 100 /
              this->history_.max_samples ()));

  if (do_dump_history)
    {
      this->history_.dump_samples ("HISTORY", gsf);
    }

  ACE_Basic_Stats stats;
  this->history_.collect_basic_stats (stats);
  stats.dump_results ("Total", gsf);

  ACE_Throughput_Stats::dump_throughput ("Total", gsf,
                                         test_end - test_start,
                                         stats.samples_count ());

  if (print_missed_invocations)
    {
      ACE_DEBUG ((LM_DEBUG, "\nMissed invocations are: "));

      for (CORBA::ULong j = 0;
           j != this->deadlines_missed_;
           ++j)
        {
          ACE_DEBUG ((LM_DEBUG,
                      "%d ",
                      this->missed_invocations_[j]));
        }

      ACE_DEBUG ((LM_DEBUG, "\n"));
    }
}

int
Paced_Worker::setup (CORBA::Environment &ACE_TRY_ENV)
{
  if (priority_setting == AFTER_THREAD_CREATION)
    {
      this->reset_priority (ACE_TRY_ENV);
      ACE_CHECK_RETURN (-1);
    }

  this->CORBA_priority_ =
    this->current_->the_priority (ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  CORBA::Boolean result =
    this->priority_mapping_.to_native (this->CORBA_priority_,
                                       this->native_priority_);
  if (!result)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error in converting CORBA priority %d to native priority\n",
                       this->CORBA_priority_),
                      -1);

  return
    start_synchronization (this->test_.in (),
                           this->synchronizers_);
}

int
Paced_Worker::svc (void)
{
  ACE_TRY_NEW_ENV
    {
      int result =
        this->setup (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (result != 0)
        return result;

      for (CORBA::ULong i = 0;
           i != this->history_.max_samples ();
           ++i)
        {
          ACE_Time_Value deadline_for_current_call =
            this->deadline_for_current_call (i);

          ACE_Time_Value time_before_call =
            ACE_OS::gettimeofday ();

          if (time_before_call > deadline_for_current_call)
            {
              this->missed_invocations_[this->deadlines_missed_] = i + 1;
              this->deadlines_missed_++;
              continue;
            }

          ACE_hrtime_t start = ACE_OS::gethrtime ();

          this->test_->method (work,
                               ACE_TRY_ENV);
          ACE_TRY_CHECK;

          ACE_hrtime_t end = ACE_OS::gethrtime ();
          this->history_.sample (end - start);

          ACE_Time_Value time_after_call =
            ACE_OS::gettimeofday ();

          if (time_after_call > deadline_for_current_call)
            continue;

          ACE_Time_Value sleep_time =
            deadline_for_current_call - time_after_call;

          ACE_OS::sleep (sleep_time);
        }

      ACE_hrtime_t test_end = ACE_OS::gethrtime ();

      done = 1;

      end_synchronization (this->synchronizers_);

      this->print_stats (test_end);
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}

class Continuous_Worker :
  public ACE_Task_Base
{
public:
  Continuous_Worker (ACE_Thread_Manager &thread_manager,
                     test_ptr test,
                     CORBA::ULong iterations,
                     RTCORBA::Current_ptr current,
                     RTCORBA::PriorityMapping &priority_mapping,
                     Synchronizers &synchronizers);

  int svc (void);
  void print_stats (ACE_Sample_History &history,
                    ACE_hrtime_t test_end);
  int setup (CORBA::Environment &ACE_TRY_ENV);
  void print_collective_stats (void);

  test_var test_;
  CORBA::ULong iterations_;
  RTCORBA::Current_var current_;
  RTCORBA::PriorityMapping &priority_mapping_;
  Synchronizers &synchronizers_;
  CORBA::Short CORBA_priority_;
  CORBA::Short native_priority_;
  ACE_Basic_Stats collective_stats_;
  ACE_hrtime_t time_for_test_;
};

Continuous_Worker::Continuous_Worker (ACE_Thread_Manager &thread_manager,
                                      test_ptr test,
                                      CORBA::ULong iterations,
                                      RTCORBA::Current_ptr current,
                                      RTCORBA::PriorityMapping &priority_mapping,
                                      Synchronizers &synchronizers)
  : ACE_Task_Base (&thread_manager),
    test_ (test::_duplicate (test)),
    iterations_ (iterations),
    current_ (RTCORBA::Current::_duplicate (current)),
    priority_mapping_ (priority_mapping),
    synchronizers_ (synchronizers),
    CORBA_priority_ (0),
    native_priority_ (0),
    collective_stats_ (),
    time_for_test_ (0)
{
}

void
Continuous_Worker::print_stats (ACE_Sample_History &history,
                                ACE_hrtime_t test_end)
{
  ACE_GUARD (ACE_SYNCH_MUTEX,
             mon,
             this->synchronizers_.worker_lock_);

  if (individual_continuous_worker_stats)
    {
      ACE_DEBUG ((LM_DEBUG,
                  "\n************ Statistics for thread %t ************\n\n"));

      ACE_DEBUG ((LM_DEBUG,
                  "Iterations = %d\n",
                  history.sample_count ()));

      if (do_dump_history)
        {
          history.dump_samples ("HISTORY", gsf);
        }

      ACE_Basic_Stats stats;
      history.collect_basic_stats (stats);
      stats.dump_results ("Total", gsf);

      ACE_Throughput_Stats::dump_throughput ("Total", gsf,
                                             test_end - test_start,
                                             stats.samples_count ());
    }

  history.collect_basic_stats (this->collective_stats_);
  ACE_hrtime_t elapsed_time_for_current_thread =
    test_end - test_start;
  if (elapsed_time_for_current_thread > this->time_for_test_)
    this->time_for_test_ = elapsed_time_for_current_thread;
}

void
Continuous_Worker::print_collective_stats (void)
{
  if (continuous_workers > 0)
    {
      ACE_DEBUG ((LM_DEBUG,
                  "\n************ Statistics for continuous workers ************\n\n"));

      ACE_DEBUG ((LM_DEBUG,
                  "Priority = %d/%d; Collective iterations = %d; Workers = %d; Average = %d\n",
                  this->CORBA_priority_,
                  this->native_priority_,
                  this->collective_stats_.samples_count (),
                  continuous_workers,
                  this->collective_stats_.samples_count () /
                  continuous_workers));

      this->collective_stats_.dump_results ("Collective", gsf);

      ACE_Throughput_Stats::dump_throughput ("Individual", gsf,
                                             this->time_for_test_,
                                             this->collective_stats_.samples_count () /
                                             continuous_workers);

      ACE_Throughput_Stats::dump_throughput ("Collective", gsf,
                                             this->time_for_test_,
                                             this->collective_stats_.samples_count ());
    }
}

int
Continuous_Worker::setup (CORBA::Environment &ACE_TRY_ENV)
{
  if (priority_setting == AFTER_THREAD_CREATION)
    {
      this->current_->the_priority (continuous_worker_priority,
                                    ACE_TRY_ENV);
      ACE_CHECK_RETURN (-1);
    }

  this->CORBA_priority_ =
    this->current_->the_priority (ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  CORBA::Boolean result =
    this->priority_mapping_.to_native (this->CORBA_priority_,
                                       this->native_priority_);
  if (!result)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error in converting CORBA priority %d to native priority\n",
                       this->CORBA_priority_),
                      -1);

  return
    start_synchronization (this->test_.in (),
                           this->synchronizers_);
}

int
Continuous_Worker::svc (void)
{
  ACE_TRY_NEW_ENV
    {
      ACE_Sample_History history (this->iterations_);

      int result =
        this->setup (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (result != 0)
        return result;

      for (CORBA::ULong i = 0;
           i != history.max_samples () && !done;
           ++i)
        {
          ACE_hrtime_t start = ACE_OS::gethrtime ();

          this->test_->method (work,
                               ACE_TRY_ENV);
          ACE_TRY_CHECK;

          ACE_hrtime_t end = ACE_OS::gethrtime ();
          history.sample (end - start);
        }

      ACE_hrtime_t test_end = ACE_OS::gethrtime ();

      end_synchronization (this->synchronizers_);

      this->print_stats (history,
                         test_end);
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}

int
main (int argc, char *argv[])
{
  Synchronizers synchronizers;

  gsf = ACE_High_Res_Timer::global_scale_factor ();

  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv, "", ACE_TRY_ENV);
      ACE_TRY_CHECK;

      int result =
        parse_args (argc, argv);
      if (result != 0)
        return result;

      fudge_priorities (orb.in ());

      CORBA::Object_var object =
        orb->string_to_object (ior, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      test_var test =
        test::_narrow (object.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      object =
        orb->resolve_initial_references ("RTCurrent",
                                         ACE_TRY_ENV);
      ACE_TRY_CHECK;

      RTCORBA::Current_var current =
        RTCORBA::Current::_narrow (object.in (),
                                   ACE_TRY_ENV);
      ACE_TRY_CHECK;

      object =
        orb->resolve_initial_references ("PriorityMappingManager",
                                         ACE_TRY_ENV);
      ACE_TRY_CHECK;

      RTCORBA::PriorityMappingManager_var mapping_manager =
        RTCORBA::PriorityMappingManager::_narrow (object.in (),
                                                  ACE_TRY_ENV);
      ACE_TRY_CHECK;

      RTCORBA::PriorityMapping &priority_mapping =
        *mapping_manager->mapping ();

      Short_Array rates;
      result =
        get_values ("client",
                    rates_file,
                    "rates",
                    rates);
      if (result != 0)
        return result;

      Short_Array invocation_priorities;
      result =
        get_values ("client",
                    invocation_priorities_file,
                    "invocation priorities",
                    invocation_priorities);
      if (result != 0)
        return result;

      if (invocation_priorities.size () != 0 &&
          invocation_priorities.size () != rates.size ())
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Number of invocation priorities (%d) != Number of rates (%d)\n",
                           invocation_priorities.size (),
                           rates.size ()),
                          -1);

      synchronizers.number_of_workers_ =
        rates.size () + continuous_workers;

      CORBA::ULong max_rate = 0;
      result =
        max_throughput (test.in (),
                        current.in (),
                        priority_mapping,
                        max_rate);
      if (result != 0)
        return result;

      CORBA::Short priority_range =
        RTCORBA::maxPriority - RTCORBA::minPriority;

      ACE_Thread_Manager paced_workers_manager;

      CORBA::ULong i = 0;
      Paced_Worker **paced_workers =
        new Paced_Worker *[rates.size ()];

      for (i = 0;
           i < rates.size ();
           ++i)
        {
          CORBA::Short priority = 0;

          if (invocation_priorities.size () == 0)
            priority =
              CORBA::Short ((priority_range /
                             double (rates.size ())) *
                            (i + 1));
          else
            priority =
              invocation_priorities[i];

          paced_workers[i] =
            new Paced_Worker (paced_workers_manager,
                              test.in (),
                              rates[i],
                              time_for_test * rates[i],
                              priority,
                              current.in (),
                              priority_mapping,
                              synchronizers);
        }

      ACE_Thread_Manager continuous_workers_manager;
      Continuous_Worker continuous_worker (continuous_workers_manager,
                                           test.in (),
                                           max_rate * time_for_test,
                                           current.in (),
                                           priority_mapping,
                                           synchronizers);
      long flags =
        THR_NEW_LWP |
        THR_JOINABLE;

      if (continuous_workers_are_rt)
        flags |=
          orb->orb_core ()->orb_params ()->scope_policy () |
          orb->orb_core ()->orb_params ()->sched_policy ();

      CORBA::Short CORBA_priority =
        continuous_worker_priority;
      CORBA::Short native_priority;
      CORBA::Boolean convert_result =
        priority_mapping.to_native (CORBA_priority,
                                    native_priority);
      if (!convert_result)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Error in converting CORBA priority %d to native priority\n",
                           CORBA_priority),
                          -1);

      int force_active = 0;

      if (priority_setting == AT_THREAD_CREATION)
        {
          result =
            continuous_worker.activate (flags,
                                        continuous_workers,
                                        force_active,
                                        native_priority);
          if (result != 0)
            return result;
        }
      else
        {
          result =
            continuous_worker.activate (flags,
                                        continuous_workers);
          if (result != 0)
            return result;
        }

      flags =
        THR_NEW_LWP |
        THR_JOINABLE |
        orb->orb_core ()->orb_params ()->scope_policy () |
        orb->orb_core ()->orb_params ()->sched_policy ();

      for (i = 0;
           i < rates.size ();
           ++i)
        {
          if (priority_setting == AT_THREAD_CREATION)
            {
              if (set_priority)
                {
                  CORBA_priority =
                    paced_workers[i]->priority_;

                  convert_result =
                    priority_mapping.to_native (CORBA_priority,
                                                native_priority);
                  if (!convert_result)
                    ACE_ERROR_RETURN ((LM_ERROR,
                                       "Error in converting CORBA priority %d to native priority\n",
                                       CORBA_priority),
                                      -1);
                }

              result =
                paced_workers[i]->activate (flags,
                                            1,
                                            force_active,
                                            native_priority);
              if (result != 0)
                return result;
            }
          else
            {
              result =
                paced_workers[i]->activate (flags);
              if (result != 0)
                return result;
            }
        }

      if (rates.size () != 0)
        {
          paced_workers_manager.wait ();
        }

      continuous_workers_manager.wait ();

      continuous_worker.print_collective_stats ();

      for (i = 0;
           i < rates.size ();
           ++i)
        {
          delete paced_workers[i];
        }
      delete[] paced_workers;

      if (shutdown_server)
        {
          test->shutdown (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Array_Base<CORBA::ULong>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Array_Base<CORBA::ULong>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
