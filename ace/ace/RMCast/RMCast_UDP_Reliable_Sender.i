// RMCast_UDP_Reliable_Sender.i,v 1.3 2000/10/25 16:54:33 coryan Exp

ACE_INLINE int
ACE_RMCast_UDP_Reliable_Sender::init (const ACE_INET_Addr &mcast_group)
{
  return this->io_udp_.init (mcast_group, ACE_Addr::sap_any);
}

ACE_INLINE int
ACE_RMCast_UDP_Reliable_Sender::has_data (void)
{
  return this->retransmission_.has_data ();
}

ACE_INLINE int
ACE_RMCast_UDP_Reliable_Sender::has_members (void)
{
  return this->membership_.has_members ();
}
