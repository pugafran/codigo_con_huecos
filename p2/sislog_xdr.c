/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "sislog.h"

bool_t
xdr_texto (XDR *xdrs, texto *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, MAXMSG))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_eventsislog (XDR *xdrs, eventsislog *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->facilidad))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->nivel))
		 return FALSE;
	 if (!xdr_texto (xdrs, &objp->msg))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_faclevel (XDR *xdrs, faclevel *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->facilidad))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->nivel))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Resultado (XDR *xdrs, Resultado *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->caso))
		 return FALSE;
	switch (objp->caso) {
	case 0:
		 if (!xdr_int (xdrs, &objp->Resultado_u.valor))
			 return FALSE;
		break;
	case 1:
		 if (!xdr_texto (xdrs, &objp->Resultado_u.msg))
			 return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}