/*************************************************************************
 *
 * ENVMODULES.C, Modules Tcl extension library
 * Copyright (C) 2018 Xavier Delaruelle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "config.h"
#include "envmodules.h"

/*----------------------------------------------------------------------
 *
 * Envmodules_ReadFileObjCmd --
 *
 *	 This function is invoked to open/read/close a regular file in a
 *	 more IO-optimized way than native Tcl commands perform by avoiding
 *	 useless lstat, fcntl and ioctl syscalls.
 *
 * Results:
 *	 A standard Tcl result.
 *
 * Side effects:
 *	 None.
 *
 *---------------------------------------------------------------------*/

int
Envmodules_ReadFileObjCmd(
   ClientData dummy,       /* Not used. */
   Tcl_Interp *interp,     /* Current interpreter. */
   int objc,               /* Number of arguments. */
   Tcl_Obj *const objv[])  /* Argument objects. */
{
   int firstline;
   const char *filename;
   int filenamelen;
   int fid;
   ssize_t len;
   char buf[READ_BUFFER_SIZE];
   Tcl_Obj *res;

   /* Parse arguments. */
   if (objc == 2) {
      firstline = 0;
   } else if (objc == 3) {
      if (Tcl_GetBooleanFromObj(interp, objv[2], &firstline) != TCL_OK) {
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION < 5
         Tcl_AppendResult(interp, "expected boolean value but got \"",
            Tcl_GetString(objv[2]), "\"", (char *) NULL);
#else
         Tcl_SetObjResult(interp, Tcl_ObjPrintf(
            "expected boolean value but got \"%s\"", Tcl_GetString(objv[2])));
#endif
         Tcl_SetErrorCode(interp, "TCL", "VALUE", "BOOLEAN", NULL);
         return TCL_ERROR;
      }
   } else {
      Tcl_WrongNumArgs(interp, 1, objv, "filename ?firstline?");
      return TCL_ERROR;
   }

   filename = Tcl_GetStringFromObj(objv[1], &filenamelen);

   /* Open file. */
   if ((fid  = open(filename, O_RDONLY)) == -1) {
      Tcl_SetErrno(errno);
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION < 5
      Tcl_AppendResult(interp, "couldn't open \"", filename, "\": ",
         Tcl_PosixError(interp), (char *) NULL);
#else
      Tcl_SetObjResult(interp, Tcl_ObjPrintf("couldn't open \"%s\": %s",
         filename, Tcl_PosixError(interp)));
#endif
      return TCL_ERROR;
   }

   /* Read file. */
   res = Tcl_NewObj();
   Tcl_IncrRefCount(res);
   /* Only read first characters to get magic cookie. */
   if (firstline == 1) {
      if ((len = read(fid, buf, FIRSTLINE_LENGTH)) > 0) {
         Tcl_AppendToObj(res, buf, len);
      }
   } else {
      while ((len = read(fid, buf, READ_BUFFER_SIZE)) > 0) {
         Tcl_AppendToObj(res, buf, len);
      }
   }
   /* Error during read. */
   if (len == -1) {
      Tcl_SetErrno(errno);
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION < 5
      Tcl_AppendResult(interp, "error reading \"", filename, "\": ",
         Tcl_PosixError(interp), (char *) NULL);
#else
      Tcl_SetObjResult(interp, Tcl_ObjPrintf("error reading \"%s\": %s",
         filename, Tcl_PosixError(interp)));
#endif
      Tcl_DecrRefCount(res);
      close(fid);
      return TCL_ERROR;
   }

   /* Close file. */
   close(fid);

   Tcl_SetObjResult(interp, res);
   Tcl_DecrRefCount(res);
   return TCL_OK;
}

/*----------------------------------------------------------------------
 *
 * Envmodules_Init --
 *
 *  Initialize the Modules commands.
 *
 * Results:
 *  TCL_OK if the package was properly initialized.
 *
 * Side effects:
 *  Adds package commands to the current interp.
 *
 *---------------------------------------------------------------------*/

DLLEXPORT int
Envmodules_Init(
   Tcl_Interp* interp      /* Tcl interpreter */
) {
    /* Require Tcl */
   if (Tcl_InitStubs(interp, "8.4", 0) == NULL) {
      return TCL_ERROR;
   }

   /* Create the provided commands */
   Tcl_CreateObjCommand(interp, "readFile", Envmodules_ReadFileObjCmd,
      (ClientData) NULL, (Tcl_CmdDeleteProc*) NULL);

   /* Provide the Envmodules package */
   if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) == TCL_ERROR) {
      return TCL_ERROR;
   }

   return TCL_OK;
}

/* vim:set tabstop=3 shiftwidth=3 expandtab autoindent: */
