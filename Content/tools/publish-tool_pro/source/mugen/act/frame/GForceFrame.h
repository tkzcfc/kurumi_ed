#pragma once

#include "GEventFrame.h"
#include "mugen/GGameDef.h"

NS_G_BEGIN

// 力施加帧
class GForceFrame : public GEventFrame
{
public:

	GForceFrame();

	virtual ~GForceFrame();

	G_SYNTHESIZE(GValueType, m_valueType, ValueType);
	G_SYNTHESIZE(GFixedVec3, m_force, Force);

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
