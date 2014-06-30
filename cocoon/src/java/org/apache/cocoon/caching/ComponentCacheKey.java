/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.caching;

/**
 * This is the cache key for one sitemap component.
 * It consists of two parts:<br>
 * a.) The component identifier - a unique handle for the sitemap
 *      component<br>
 * b.) The cache key - a key, generated by the component, which
 *      is unique inside the component space.
 *
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:08 $
 */
public final class ComponentCacheKey implements java.io.Serializable {

    public static final int ComponentType_Generator   = 1;
    public static final int ComponentType_Transformer = 2;
    public static final int ComponentType_Serializer  = 3;
    public static final int ComponentType_Reader      = 4;

    private String toStringValue;

    public ComponentCacheKey(int    componentType,
                             String componentIdentifier,
                             long   cacheKey) {
        // generate the toString value
        StringBuffer buffer = new StringBuffer("CCK:");
        buffer.append(componentType)
               .append('-')
               .append(componentIdentifier)
               .append('-')
               .append(cacheKey);
        this.toStringValue = buffer.toString();
    }

    public String toString() {
        return this.toStringValue;
    }

    public boolean equals(Object object) {
        return this.toStringValue.equals(object.toString());
    }

    public int hashCode() {
        return this.toStringValue.hashCode();
    }
}