/* handle the interactive controls */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#include <Xm/Xm.h>
#include <X11/keysym.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "strops.h"
#include "misc.h"
#include "configfile.h"
#include "telstatshm.h"
#include "xtools.h"

#include "widgets.h"
#include "xobs.h"

static Widget home_w;		/* main Home dialog */
static Widget limit_w;		/* main Limit dialog */

static void homeCreateW(void);
static void limitCreateW(void);
static void homeCB (Widget w, XtPointer client, XtPointer call);
static void limitCB (Widget w, XtPointer client, XtPointer call);
static void hlSetup(void);
static void closeCB (Widget w, XtPointer client, XtPointer call);

/* give ability to home or limit each axis */
/* name for each fifo */
typedef enum {
    H_AX, D_AX, O_AX, N_AX
} HLAxis;
typedef struct {
    char *name;		/* descriptive name on button */
    FifoId fid;		/* which channel to use */
    char telaxcode;	/* if Tel_Id, one of the axis codes, else '\0' */
    Widget hpbw;	/* home PB widget */
    Widget lpbw;	/* limit PB widget */
} HLInfo;
static HLInfo hl_info[N_AX] = {
    {"Scope HA/Az",   Tel_Id,    'H'},
    {"Scope Dec/Alt", Tel_Id,    'D'},
    {"Focus",         Focus_Id}
};

void
g_stop (Widget w, XtPointer client, XtPointer call)
{
	/* important enough to risk no confirmation
	if (!rusure (toplevel_w, "stop everything"))
	    return;
	*/

	msg ("All stop");

	/* issue stops to all fifos */
	stopAllDevices();

	/* reset the paddle */
	pad_reset();
}

void
g_exit (Widget w, XtPointer client, XtPointer call)
{
	int qonnow = rusure_geton();

	/* always require ack to exit */
	rusure_seton (1);
	if (!rusure (toplevel_w, "exit this program entirely")) {
	    rusure_seton (qonnow);
	    return;
	}

	die();
}

void
g_confirm (Widget w, XtPointer client, XtPointer call)
{
	/* let them turn confirmations back on without a confirmation */
	if (!XmToggleButtonGetState(w)) {
	    setLt (g_w[SCLT_W], LTIDLE);
	    rusure_seton(1);
	    return;
	}

	if (rusure (toplevel_w, "turn off confirmation messages")) {
	    setLt (g_w[SCLT_W], LTWARN);
	    rusure_seton(0);
	} else
	    XmToggleButtonSetState (w, False, True);
}

void
g_home (Widget w, XtPointer client, XtPointer call)
{
	/* create both for hlsetup() */
	if (!home_w)
	    homeCreateW();
	if (!limit_w)
	    limitCreateW();

	if (!XtIsManaged(home_w)) {
	    hlSetup();
	    XtManageChild (home_w);
	} else
	    XtUnmanageChild (home_w);
}

void
g_limit (Widget w, XtPointer client, XtPointer call)
{
	/* create both for hlsetup() */
	if (!limit_w)
	    limitCreateW();
	if (!home_w)
	    homeCreateW();

	if (!XtIsManaged(limit_w)) {
	    hlSetup();
	    XtManageChild (limit_w);
	} else
	    XtUnmanageChild (limit_w);
}

void
g_paddle (Widget w, XtPointer client, XtPointer call)
{
	pad_manage();
}

/* create the dialog to allow finding each home axis */
static void
homeCreateW()
{
	Widget rc_w, w;
	int i;

	/* make the main form */
	home_w = XmCreateFormDialog (toplevel_w, "HD", NULL, 0);
	XtVaSetValues (home_w,
	    XmNverticalSpacing, 10,
	    XmNhorizontalSpacing, 10,
	    XmNautoUnmanage, False,
	    NULL);
	XtVaSetValues (XtParent(home_w),
	    XmNtitle, "Find Homes",
	    NULL);

	/* put a rc in it */
	rc_w = XmCreateRowColumn (home_w, "HRC", NULL, 0);
	XtManageChild (rc_w);
	XtVaSetValues (rc_w,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNisAligned, True,
	    XmNentryAlignment, XmALIGNMENT_CENTER,
	    NULL);

	/* make one for each axis */
	for (i = 0; i < N_AX; i++) {
	    w = XmCreatePushButton (rc_w, "PB", NULL, 0);
	    XtAddCallback (w, XmNactivateCallback, homeCB, (XtPointer)(1<<i));
	    XtManageChild (w);
	    wlprintf (w, "%s", hl_info[i].name);
	    hl_info[i].hpbw = w;
	}

	/* make one for "all" */
	w = XmCreateSeparator (rc_w, "Pad", NULL, 0);
	XtVaSetValues (w,
	    XmNheight, 10,
	    XmNseparatorType, XmNO_LINE,
	    NULL);
	XtManageChild (w);
	w = XmCreatePushButton (rc_w, "PB", NULL, 0);
	XtAddCallback(w, XmNactivateCallback,homeCB,(XtPointer)((1<<N_AX)-1));
	XtManageChild (w);
	wlprintf (w, "%s", "All");

	/* close */
	w = XmCreateSeparator (rc_w, "Pad", NULL, 0);
	XtVaSetValues (w,
	    XmNheight, 10,
	    XmNseparatorType, XmNO_LINE,
	    NULL);
	XtManageChild (w);
	w = XmCreatePushButton (rc_w, "Close", NULL, 0);
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback, closeCB,(XtPointer)home_w);
}

/* create the dialog to allow finding each limit axis */
static void
limitCreateW()
{
	Widget rc_w, w;
	int i;

	/* make the main form */
	limit_w = XmCreateFormDialog (toplevel_w, "HD", NULL, 0);
	XtVaSetValues (limit_w,
	    XmNverticalSpacing, 10,
	    XmNhorizontalSpacing, 10,
	    XmNautoUnmanage, False,
	    NULL);
	XtVaSetValues (XtParent(limit_w),
	    XmNtitle, "Find Limits",
	    NULL);

	/* put a rc in it */
	rc_w = XmCreateRowColumn (limit_w, "HRC", NULL, 0);
	XtManageChild (rc_w);
	XtVaSetValues (rc_w,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNisAligned, True,
	    XmNentryAlignment, XmALIGNMENT_CENTER,
	    NULL);

	/* make one for each axis */
	for (i = 0; i < N_AX; i++) {
	    w = XmCreatePushButton (rc_w, "PB", NULL, 0);
	    XtAddCallback (w, XmNactivateCallback, limitCB, (XtPointer)(1<<i));
	    XtManageChild (w);
	    wlprintf (w, "%s", hl_info[i].name);
	    hl_info[i].lpbw = w;
	}

	/* make one for "all" */
	w = XmCreateSeparator (rc_w, "Pad", NULL, 0);
	XtVaSetValues (w,
	    XmNheight, 10,
	    XmNseparatorType, XmNO_LINE,
	    NULL);
	XtManageChild (w);
	w = XmCreatePushButton (rc_w, "PB", NULL, 0);
	XtAddCallback(w, XmNactivateCallback,limitCB,(XtPointer)((1<<N_AX)-1));
	XtManageChild (w);
	wlprintf (w, "%s", "All");

	/* close */
	w = XmCreateSeparator (rc_w, "Pad", NULL, 0);
	XtVaSetValues (w,
	    XmNheight, 10,
	    XmNseparatorType, XmNO_LINE,
	    NULL);
	XtManageChild (w);
	w = XmCreatePushButton (rc_w, "Close", NULL, 0);
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback, closeCB,(XtPointer)limit_w);
}

/* customize home_w and limit_w according to what is currently connected */
static void
hlSetup()
{
	XtSetSensitive (hl_info[H_AX].hpbw, !!HMOT->have);
	XtSetSensitive (hl_info[D_AX].hpbw, !!DMOT->have);
	XtSetSensitive (hl_info[O_AX].hpbw, !!OMOT->have);

	XtSetSensitive (hl_info[H_AX].lpbw, !!HMOT->have && !!HMOT->havelim);
	XtSetSensitive (hl_info[D_AX].lpbw, !!DMOT->have && !!DMOT->havelim);
	XtSetSensitive (hl_info[O_AX].lpbw, !!OMOT->have && !!OMOT->havelim);
}

/* called from either home's or limit's close PB.
 * client is form dialog to close.
 */
static void
closeCB (Widget w, XtPointer client, XtPointer call)
{
	XtUnmanageChild ((Widget)client);
}

/* called from any of the PB in the home dialog.
 * client is a bitmask of h_info indexes's for which to find home.
 */
static void
homeCB (Widget w, XtPointer client, XtPointer call)
{
	int idmask = (int)client;
	char axcodes[N_AX];
	int naxcodes;
	int i;
	
	/* N.B. must build *one* composite fifo command to Tel_Id */
	
	naxcodes = 0;
	for (i = 0; i < N_AX; i++) {
	    HLInfo *hip = &hl_info[i];
	    if ((idmask & (1<<i)) && XtIsSensitive(hip->hpbw)) { /*lazy "have"*/
		if (hip->telaxcode)
		    axcodes[naxcodes++] = hip->telaxcode;
		else
		    fifoMsg (hip->fid, "home");
		msg ("Seeking %s home", hip->name);
	    }
	}

	if (naxcodes > 0)
	    fifoMsg (Tel_Id, "home%.*s", naxcodes, axcodes);
}

/* called from any of the PB in the limit dialog.
 * client is a bitmask of h_info indexes's for which to find limit.
 */
static void
limitCB (Widget w, XtPointer client, XtPointer call)
{
	int idmask = (int)client;
	char axcodes[N_AX];
	int naxcodes;
	int i;

	/* N.B. must build *one* composite fifo command to Tel_Id */

	naxcodes = 0;
	for (i = 0; i < N_AX; i++) {
	    HLInfo *hip = &hl_info[i];
	    if ((idmask & (1<<i)) && XtIsSensitive(hip->lpbw)) { /*lazy "have"*/
		if (hip->telaxcode)
		    axcodes[naxcodes++] = hip->telaxcode;
		else
		    fifoMsg (hip->fid, "limits");
		msg ("Seeking %s limits", hip->name);
	    }
	}

	if (naxcodes > 0)
	    fifoMsg (Tel_Id, "limits%.*s", naxcodes, axcodes);
}
