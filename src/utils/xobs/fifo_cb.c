/* sets up FIFOs for xobs and implements related callbacks */

/* Heavily modified to pull out and generalize FIFO comm code for
   use in other programs (like joystickd) - KMI 8/4/2005 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Xm/Xm.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "configfile.h"
#include "misc.h"
#include "strops.h"
/* #include "db.h" */
#include "cliserv.h"
#include "telenv.h"
#include "telstatshm.h"
#include "xtools.h"

#include "widgets.h"
#include "xobs.h"

static void tel_rd_cb(XtPointer client, int *fdp, XtInputId *idp);
static void focus_rd_cb(XtPointer client, int *fdp, XtInputId *idp);
static void dome_rd_cb(XtPointer client, int *fdp, XtInputId *idp);
static void cover_rd_cb(XtPointer client, int *fdp, XtInputId *idp);

/* this is used to describe the several FIFOs used to communicate with
 * the telescoped.
 */
typedef struct
{
    int fifo_index;
    XtInputCallbackProc cb; /* callback to process input from this fifo */
    XtInputId id;           /* X connection to fifo */
} FifoCallback;

/* list of fifos to the control daemons */
static FifoCallback fifocb[] = {
    {Tel_Id, tel_rd_cb},
    {Focus_Id, focus_rd_cb},
    {Dome_Id, dome_rd_cb},
    {Cover_Id, cover_rd_cb},
};

/* make connections to daemons.
 * N.B. do nothing gracefully if connections are already ok.
 */
void initPipesAndCallbacks()
{
    FifoCallback *fcb;

    setFifoErrorCallback(die); // On fifo errors, call our own die() function
                               // which performs some cleanups before exiting

    openFIFOs();

    for (fcb = fifocb; fcb < &fifocb[numFifos]; fcb++)
    {
        FifoInfo fi = getFIFO(fcb->fifo_index);
        if (fi.fdopen && fcb->id == 0)
            fcb->id = XtAppAddInput(app, fi.fd[0], (XtPointer)XtInputReadMask, fcb->cb, 0);
    }
}

void closePipesAndCallbacks()
{
    FifoCallback *fcb;

    for (fcb = fifocb; fcb < &fifocb[numFifos]; fcb++)
    {
        if (fcb->id != 0)
            XtRemoveInput(fcb->id);
        fcb->id = 0;
    }

    closeFIFOs();
}

/* called whenever we get input from the Tel fifo */
/* ARGSUSED */
static void tel_rd_cb(client, fdp, idp) XtPointer client; /* unused */
int *fdp;                                                 /* pointer to file descriptor */
XtInputId *idp;                                           /* pointer to input id */
{
    char buf[1024];

    fifoRead(Tel_Id, buf, sizeof(buf));
    msg("Telescope: %s", buf);
    updateStatus(1);
}

/* called whenever we get input from the Focus fifo */
/* ARGSUSED */
static void focus_rd_cb(client, fdp, idp) XtPointer client; /* unused name */
int *fdp;                                                   /* pointer to file descriptor */
XtInputId *idp;                                             /* pointer to input id */
{
    char buf[1024];
    int s;

    s = fifoRead(Focus_Id, buf, sizeof(buf));
    msg("Focus: %s", buf);
    updateStatus(1);
}

/* called whenever we get input from the Dome fifo */
/* ARGSUSED */
static void dome_rd_cb(client, fdp, idp) XtPointer client; /* file name */
int *fdp;                                                  /* pointer to file descriptor */
XtInputId *idp;                                            /* pointer to input id */
{
    char buf[1024];

    fifoRead(Dome_Id, buf, sizeof(buf));
    msg("Dome: %s", buf);

    updateStatus(1);
}

static void cover_rd_cb(client, fdp, idp) XtPointer client; /* file name */
int *fdp;                                                   /* pointer to file descriptor */
XtInputId *idp;                                             /* pointer to input id */
{
    char buf[1024];

    fifoRead(Cover_Id, buf, sizeof(buf));
    msg("Cover: %s", buf);

    updateStatus(1);
}
