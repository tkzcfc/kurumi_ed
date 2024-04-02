#include "GForceFrame.h"
#include "mugen/component/GRigidBodyComponent.h"
#include "mugen/component/GDataComponent.h"

NS_G_BEGIN

GForceFrame::GForceFrame()
{
	m_type = GFrameType::FORCE_FRAME;
	setTriggerMaxCount(0);
	setTriggerInterval(0);
}

GForceFrame::~GForceFrame()
{}

void GForceFrame::onEnter(int32_t currentFrameIndex)
{
	m_pRigidBodyComponent = G_GetComponent(m_pEntity, GRigidBodyComponent);
	m_pGDataComponent = G_GetComponent(m_pEntity, GDataComponent);
}

void GForceFrame::onEmit()
{
	if (m_valueType == GValueType::VALUE_ABSOLUTE)
	{
		m_pRigidBodyComponent->applyForce(m_force);
	}
	else if (m_valueType == GValueType::VALUE_PERCENTAGE)
	{
		auto force = m_pGDataComponent->force;
		force.x *= m_force.x;
		force.y *= m_force.y;
		force.z *= m_force.z;
		m_pRigidBodyComponent->applyForce(force);
	}
	else
	{
		G_ASSERT(0);
	}
}

void GForceFrame::serialize(GByteBuffer& byteBuffer)
{
	GEventFrame::serialize(byteBuffer);

	byteBuffer.writeInt32((int32_t)m_valueType);
	byteBuffer.writeFixedPoint(m_force.x);
	byteBuffer.writeFixedPoint(m_force.y);
	byteBuffer.writeFixedPoint(m_force.z);
}

bool GForceFrame::deserialize(GByteBuffer& byteBuffer)
{
	do
	{
		G_BREAK_IF(!GEventFrame::deserialize(byteBuffer));

		auto value = byteBuffer.readInt32();
		if (value < (int32_t)GValueType::VALUE_PERCENTAGE || value >(int32_t)GValueType::VALUE_ABSOLUTE)
		{
			G_ASSERT(0);
			break;
		}
		m_valueType = (GValueType)value;

		G_BREAK_IF(!byteBuffer.getFixedPoint(m_force.x));
		G_BREAK_IF(!byteBuffer.getFixedPoint(m_force.y));
		G_BREAK_IF(!byteBuffer.getFixedPoint(m_force.z));

		return true;
	} while (0);
	return false;
}

NS_G_END