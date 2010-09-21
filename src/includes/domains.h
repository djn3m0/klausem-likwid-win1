/*
 * ===========================================================================
 *
 *       Filename:  domains.h
 *
 *    Description:  Header File domains Module. 
 *
 *        Version:  1.0
 *        Created:  30/04/2010
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
 *      Copyright:  Copyright (c) 2010, Jan Treibig
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ===========================================================================
 */

#ifndef DOMAINS_H
#define DOMAINS_H

#include <types.h>

extern void domains_init();
extern void domains_finalize();

extern const AffinityDomain* domains_getDomain(bstring domain);
extern void domains_printDomains();

#endif /*DOMAINS_H*/

