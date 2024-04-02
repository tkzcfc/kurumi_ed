#include "GImpulseFrame.h"
#include "mugen/component/GRigidBodyComponent.h"
#include "mugen/component/GDataComponent.h"

NS_G_BEGIN

GImpulseFrame::GImpulseFrame()
{
	m_type = GFrameType::IMPULSE_FRAME;
	setTriggerMaxCount(1);
}

GImpulseFrame::~GImpulseFrame()
{}

void GImpulseFrame::onEnter(int32_t currentFrameIndex)
{
	m_pRigidBodyComponent = G_GetComponent(m_pEntity, GRigidBodyComponent);
	m_pGDataComponent = G_GetComponent(m_pEntity, GDataComponent);
}

void GImpulseFrame::onEmit()
{
	if (m_valueType == GValueType::VALUE_ABSOLUTE)
	{
		m_pRigidBodyComponent->applyImpulse(m_impulse);
	}
	else if (m_valueType == GValueType::VALUE_PERCENTAGE)
	{
		auto impulse = m_pGDataComponent->impulse;
		impulse.x *= m_impulse.x;
		impulse.y *= m_impulse.y;
		impulse.z *= m_impulse.z;
		m_pRigidBodyComponent->applyImpulse(impulse);
	}
	else
	{
		G_ASSERT(0);
	}
}

void GImpulseFrame::serialize(GByteBuffer& byteBuffer)
{
	GEventFrame::serialize(byteBuffer);

	byteBuffer.writeInt32((int32_t)m_valueType);
	byteBuffer.writeFixedPoint(m_impulse.x);
	byteBuffer.writeFixedPoint(m_impulse.y);
	byteBuffer.writeFixedPoint(m_impulse.z);
}

bool GImpulseFrame::deserialize(GByteBuffer& byteBuffer)
{
	do
	{
		G_BREAK_IF(!GEventFrame::deserialize(byteBuffer));

		auto value = byteBuffer.readInt32();
		if (value < (int32_t)GValueType::VALUE_PERCENTAGE || value >(int32_t)GValueType::VALUE_ABSOLUTE)
		{
			break;
		}
		m_valueType = (GValueType)value;

		G_BREAK_IF(!byteBuffer.getFixedPoint(m_impulse.x));
		G_BREAK_IF(!byteBuffer.getFixedPoint(m_impulse.y));
		G_BREAK_IF(!byteBuffer.getFixedPoint(m_impulse.z));

		return true;
	} while (0);
	return false;
}

NS_G_END