#include "NoSpread.h"

#include "NoSpreadProjectile/NoSpreadProjectile.h"

bool CNoSpread::ShouldRun(CTFPlayer* pLocal, CTFWeaponBase* pWeapon)
{
	if (!Vars::Aimbot::Projectile::NoSpread.Value
		|| !pWeapon || !pLocal->CanAttack())
		return false;

	return true;
}

void CNoSpread::Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
{
	if (!ShouldRun(pLocal, pWeapon))
		return;

	F::NoSpreadProjectile.Run(pLocal, pWeapon, pCmd);
}