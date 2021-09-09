#include "StdAfx.h"
#include "KinematicsBase.h"

BOOL GetEquivalentOrientation(int nIndex,
	double rx,
	double ry,
	double rz,
	double* equiv_rx,
	double* equiv_ry,
	double* equiv_rz)
{
	*equiv_rx = rx = fmod(rx, 2.0 * M_PI);
	*equiv_ry = ry = fmod(ry, 2.0 * M_PI);
	*equiv_rz = rz = fmod(rz, 2.0 * M_PI);

	switch( nIndex )
	{
	case 0:
		*equiv_rx += ((rx < 0) ? 2.0 * M_PI: -2.0 * M_PI);
		break;

	case 1:
		*equiv_rz += ((rz < 0) ? 2.0 * M_PI: -2.0 * M_PI);
		break;

	case 2:
		*equiv_rx += ((rx < 0) ? 2.0 * M_PI: -2.0 * M_PI);
		*equiv_rz += ((rz < 0) ? 2.0 * M_PI: -2.0 * M_PI);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

void __stdcall  V6_InitExtension(void* pvExtension)
{
	if( pvExtension )
	{
		EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );
		ext->last_posture  = 0;
	}
}

void __stdcall  V6_PreProcessMotionCommand(int nGroup,
	int nSynAxisNum,
	const double* pdfCurCPos,
	const double* pdfTgtCPos,
	double* pdfOffset,
	void* pvExtension)
{
	if( nGroup != 0 || nSynAxisNum != 6 )
		return;
	int  idx = 0;
	double  min_orient_dist = 1e+100;
	double  rx, ry, rz, min_rx, min_ry, min_rz;

	rx = min_rx = pdfCurCPos[3];
	ry = min_ry = pdfCurCPos[4];
	rz = min_rz = pdfCurCPos[5];

	do
	{
		double  orient_dist = sqrt(
			((pdfTgtCPos[3] - rx) * (pdfTgtCPos[3] - rx)) +
			((pdfTgtCPos[4] - ry) * (pdfTgtCPos[4] - ry)) +
			((pdfTgtCPos[5] - rz) * (pdfTgtCPos[5] - rz)) );

		if( orient_dist < min_orient_dist )
		{
			min_orient_dist = orient_dist;
			min_rx = rx;
			min_ry = ry;
			min_rz = rz;
		}

	} while( GetEquivalentOrientation(
		idx++,
		pdfCurCPos[3],
		pdfCurCPos[4],
		pdfCurCPos[5],
		&rx,
		&ry,
		&rz) );


	memset(pdfOffset, 0, sizeof(double) * nSynAxisNum);

	pdfOffset[3] = min_rx - pdfCurCPos[3];
	pdfOffset[4] = min_ry - pdfCurCPos[4];
	pdfOffset[5] = min_rz - pdfCurCPos[5];
}

DWORD  DeterminePosture(BOOL bArmLefty, int nJointNum, const double* pdfJointAngle, double dh_thredshold )
{
	DWORD  posture = (bArmLefty) ? POST_ARM_LEFTY: 0;

	double DH_elbow_post_threshold = dh_thredshold;
	// Elbow & Flip
	if( bArmLefty )
	{
		if( pdfJointAngle[4] > 0 )
			posture |= POST_NONFLIP;

		if( pdfJointAngle[2] <= DH_elbow_post_threshold )
			posture |= POST_ELBOW_BELOW;
	}
	else
	{
		if( pdfJointAngle[4] < 0 )
			posture |= POST_NONFLIP;

		if( pdfJointAngle[2] >= DH_elbow_post_threshold )
			posture |= POST_ELBOW_BELOW;
	}

	// theta 4 (single/double)
	if( pdfJointAngle[3] <= -M_PI || pdfJointAngle[3] > M_PI )
		posture |= POST_J4DOUBLE;

	// theta 6 (single/double)
	if( pdfJointAngle[5] <= -M_PI || pdfJointAngle[5] > M_PI )
		posture |= POST_J6DOUBLE;

	return posture;
}
