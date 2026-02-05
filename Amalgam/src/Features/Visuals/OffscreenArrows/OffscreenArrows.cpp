#include "OffscreenArrows.h"

#include "../Groups/Groups.h"

void COffscreenArrows::DrawArrowTo(const Vec3& vFromPos, const Vec3& vToPos, Color_t tColor, int iOffset, float flMaxDistance)
{
	const float flDist = vFromPos.DistTo(vToPos);

	// base fade
	float flFade = Math::RemapVal(
		flDist,
		flMaxDistance,
		flMaxDistance * 0.9f,
		0.f,
		1.f
	);
	flFade = std::clamp(flFade, 0.f, 1.f);

	// start flashing when closer
	const float flFlashStart = flMaxDistance * 0.6f;

	float flFlash = 0.f;
	if (flDist < flFlashStart)
	{
		float flTime = SDK::PlatFloatTime();

		float flSpeed = Math::RemapVal(
			flDist,
			flFlashStart,
			flFlashStart * 0.25f,
			6.f,
			28.f
		);
		flSpeed = std::clamp(flSpeed, 6.f, 28.f);

		flFlash = sinf(flTime * flSpeed) * 0.5f + 0.5f;
	}

	// blend color to black
	tColor.r = byte(Math::Lerp(float(tColor.r), 0.f, flFlash));
	tColor.g = byte(Math::Lerp(float(tColor.g), 0.f, flFlash));
	tColor.b = byte(Math::Lerp(float(tColor.b), 0.f, flFlash));
	tColor.a = byte(flFade * 255.f);

	if (!tColor.a)
		return;

	Vec2 vCenter =
	{
		H::Draw.m_nScreenW * 0.5f,
		H::Draw.m_nScreenH * 0.5f
	};

	Vec3 vScreen{};
	SDK::W2S(vToPos, vScreen, true);

	Vec3 vAngle = Math::VectorAngles(
		{
			vScreen.x - vCenter.x,
			vScreen.y - vCenter.y,
			0.f
		});

	const float flRad = DEG2RAD(vAngle.y);
	const float flRadius = float(iOffset);
	const float flArc = DEG2RAD(42.f);
	const float flThickness = H::Draw.Scale(4.f);

	const int iSegments = 24;
	const float a0 = flRad - flArc * 0.5f;
	const float a1 = flRad + flArc * 0.5f;

	for (int i = 0; i < iSegments; i++)
	{
		float t0 = float(i) / iSegments;
		float t1 = float(i + 1) / iSegments;

		float ang0 = a0 + (a1 - a0) * t0;
		float ang1 = a0 + (a1 - a0) * t1;

		Vec2 o0 = { cosf(ang0), sinf(ang0) };
		Vec2 o1 = { cosf(ang1), sinf(ang1) };

		Vec2 p0 = vCenter + o0 * (flRadius - flThickness);
		Vec2 p1 = vCenter + o0 * (flRadius + flThickness);
		Vec2 p2 = vCenter + o1 * (flRadius + flThickness);
		Vec2 p3 = vCenter + o1 * (flRadius - flThickness);

		H::Draw.FillPolygon(
			{
				{ p0 },
				{ p1 },
				{ p2 },
				{ p3 }
			},
			tColor);
	}
}


void COffscreenArrows::Store(CTFPlayer* pLocal)
{
	m_mCache.clear();
	if (!F::Groups.GroupsActive())
		return;

	for (auto& [pEntity, pGroup] : F::Groups.GetGroup(false))
	{
		if (!pGroup->m_bOffscreenArrows
			|| pEntity->entindex() == I::EngineClient->GetLocalPlayer())
			continue;

		ArrowCache_t& tCache = m_mCache[pEntity];
		tCache.m_tColor = F::Groups.GetColor(pEntity, pGroup);
		tCache.m_iOffset = pGroup->m_iOffscreenArrowsOffset;
		tCache.m_flMaxDistance = pGroup->m_flOffscreenArrowsMaxDistance;
	}
}

void COffscreenArrows::Draw(CTFPlayer* pLocal)
{
	if (m_mCache.empty())
		return;

	Vec3 vLocalPos = pLocal->GetEyePosition();
	for (auto& [pEntity, tCache] : m_mCache)
		DrawArrowTo(vLocalPos, pEntity->GetCenter(), tCache.m_tColor, tCache.m_iOffset, tCache.m_flMaxDistance);
}