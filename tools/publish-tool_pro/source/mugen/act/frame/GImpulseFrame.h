#pragma once

#include "GEventFrame.h"
#include "mugen/GGameDef.h"

NS_G_BEGIN

// 冲力施加帧
class GImpulseFrame : public GEventFrame
{
public:

	GImpulseFrame();

	virtual ~GImpulseFrame();

	G_SYNTHESIZE(GValueType, m_valueType, ValueType);
	G_SYNTHESIZE(GFixedVec3, m_impulse, Impulse);

	virtual void onEnter(int32_t currentFrameIndex) override;

	virtual void onEmit() override;

public:

	virtual void serialize(GByteBuffer& byteBuffer) override;

	virtual bool deserialize(GByteBuffer& byteBuffer) override;

private:
	class GRigidBodyComponent* m_pRigidBodyComponent;
	class GDataComponent* m_pGDataComponent;
};

NS_G_END
