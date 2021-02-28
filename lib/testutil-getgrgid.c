/*************************************************************************
 *
 * TESTUTIL-GETGRGID.C, Superseded getgrgid function for test purpose
 * Copyright (C) 2020-2021 Xavier Delaruelle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include <stddef.h>
#include <grp.h>

struct group *getgrgid(gid_t gid)
{
    return NULL;
}

/* vim:set tabstop=3 shiftwidth=3 expandtab autoindent: */
