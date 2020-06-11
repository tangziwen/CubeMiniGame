#include "BearPart.h"
#include "Interface/Drawable3D.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Texture/TextureMgr.h"
#include "NodeEditorNodes/BearingPartNode.h"
#include "GameUISystem.h"
#include "GamePart.h"
#include "Island.h"
#include "Collision/PhysicsMgr.h"
#include "Utility/math/TbaseMath.h"
#include "Rendering/Renderer.h"
#include "BuildingSystem.h"
#include "3D/Model/Model.h"
#include "Base/TranslationMgr.h"
#include "3D/Primitive/CubePrimitive.h"

// #include "EngineSrc/Collision/Physics6DOFConstraint.h"
namespace tzw
{
	float blockSize = 0.05;
// BearPart::BearPart()
// {
// 	m_a = nullptr;
// 	m_b = nullptr;
// 	m_isFlipped = false;
// 	m_node = nullptr;
// 	m_constrain = nullptr;
// 	
//
// 	m_isSteering = false;
// 	m_isAngleLimit = false;
// 	m_angleLimitLow = -30.0f;
// 	m_angleLimitHigh = 30.0f;
// 	//forward backward
// 	addAttachment(new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this));
// 	addAttachment(new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this));
// 	BearPart::generateName();
// 	
// 	auto nodeEditor = GameUISystem::shared()->getNodeEditor();
// 	m_graphNode = new BearingPartNode(this);
// 	nodeEditor->addNode(m_graphNode);
// 	m_xrayMat = Material::createFromTemplate("PartXRay");
// 	
// 	// create a indicate model
// 	auto cylinderIndicator = new CylinderPrimitive(0.15, 0.15, 0.1);
// 	cylinderIndicator->setColor(vec4(1.0, 1.0, 1.0, 1.0));
// 	m_node = cylinderIndicator;
// 	m_xrayMat->setTex("DiffuseMap", cylinderIndicator->getTopBottomMaterial()->getTex("diffuseMap"));
// 	cylinderIndicator->onSubmitDrawCommand = [cylinderIndicator, this](RenderCommand::RenderType passType)
// 	{
// 		if(BuildingSystem::shared()->isIsInXRayMode())
// 		{
// 			RenderCommand command(cylinderIndicator->getMesh(), this->m_xrayMat, passType);
// 			cylinderIndicator->setUpCommand(command);
// 			command.setRenderState(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR);
// 			Renderer::shared()->addRenderCommand(command);
//
// 			RenderCommand command2(cylinderIndicator->getTopBottomMesh(), this->m_xrayMat, passType);
// 			cylinderIndicator->setUpCommand(command2);
// 			command2.setRenderState(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR);
// 			Renderer::shared()->addRenderCommand(command2);
// 		}
// 	};
// }

BearPart::BearPart(std::string itemName)
{
	m_a = nullptr;
	m_b = nullptr;
	m_isFlipped = false;
	m_node = nullptr;
	m_constrain = nullptr;
	

	m_isSteering = false;
	m_isAngleLimit = false;
	m_angleLimitLow = -30.0f;
	m_angleLimitHigh = 30.0f;


	GamePart::initFromItemName(itemName);
	
	BearPart::generateName();

	m_xrayMat = Material::createFromTemplate("PartXRay");
	
	auto cylinderIndicator = static_cast<Model *> (m_node);
	m_xrayMat->setTex("DiffuseMap", cylinderIndicator->getMat(0)->getTex("DiffuseMap"));
	cylinderIndicator->onSubmitDrawCommand = [cylinderIndicator, this](RenderCommand::RenderType passType)
	{
		if(BuildingSystem::shared()->isIsInXRayMode())
		{
			RenderCommand command(cylinderIndicator->getMesh(0), this->m_xrayMat, passType);
			cylinderIndicator->setUpCommand(command);
			command.setRenderState(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR);
			Renderer::shared()->addRenderCommand(command);
		}
	};
}

	void BearPart::updateFlipped()
{
	if(!m_node) return;
	auto model = static_cast<Model *>(m_node);
	Texture * tex;
	if(m_isFlipped)
	{
		tex = TextureMgr::shared()->getByPath("Blocks/Bearing/diffuse_inverted.png");
	}
	else
	{
		tex = TextureMgr::shared()->getByPath("Blocks/Bearing/diffuse.png");
	}
	model->getMat(0)->setTex("DiffuseMap", tex);
	m_xrayMat->setTex("DiffuseMap", tex);
}

int BearPart::getAttachmentCount()
{
	return 2;
}

BearPart::~BearPart()
{
	auto nodeEditor = GameUISystem::shared()->getNodeEditor();
	nodeEditor->removeNode(m_graphNode);
	delete m_graphNode;
	if(m_constrain) 
	{
		PhysicsMgr::shared()->removeConstraint(m_constrain);
		delete m_constrain;
	}
	
}

GraphNode* BearPart::getGraphNode() const
{
	return m_graphNode;
}

void BearPart::load(rapidjson::Value& constraintData)
{
	m_isFlipped = constraintData["isFlipped"].GetBool();
	updateFlipped();
    static_cast<ResNode *> (m_graphNode)->syncName();
}

void BearPart::setIsSteering(bool isSteering)
{
	m_isSteering = isSteering;
	if(m_constrain && m_isEnablePhysics)
	{
		if(m_isSteering)
		{
			m_constrain->enableAngularMotor(true, 0.0f, 10000.0f);
		}
		else 
		{
			m_constrain->enableAngularMotor(false, 0.0f, 10000.0f);
		}
	}

}

bool BearPart::getIsSteering() const
{
	return m_isSteering;
}

void BearPart::setAngleLimit(bool isAngleLimit, float low, float high)
{
	m_isAngleLimit = isAngleLimit;
	m_angleLimitLow = low;
	m_angleLimitHigh = high;
	if(m_constrain && m_isEnablePhysics)
	{
		if(m_isAngleLimit)
		{
			m_constrain->setLimit(m_angleLimitLow* (TbaseMath::PI_OVER_180), m_angleLimitHigh * (TbaseMath::PI_OVER_180));
		}
		else 
		{
			m_constrain->setLimit(1, -1);
		}
	}
}

void BearPart::getAngleLimit(bool& isAngleLimit, float& low, float& high) const
{
	isAngleLimit = m_isAngleLimit;
	low = m_angleLimitLow;
	high = m_angleLimitHigh;
}

void BearPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, TRC(u8"轴承 %s"),genShortName().c_str());
	setName(formatName);
}

	GamePartType BearPart::getType()
	{
		return GamePartType::GAME_PART_BEARING;
	}

	void BearPart::enableAngularMotor(bool enableMotor, float targetVelocity, float maxMotorImpulse)
	{
		if(m_constrain)
		{
			float factor = m_isFlipped?-1.0f:1.0f;
			m_constrain->enableAngularMotor(enableMotor, targetVelocity * factor, maxMotorImpulse);
		}
	}

	void BearPart::drawInspect()
	{
		if(drawInspectNameEdit())
		{
			static_cast<ResNode *>(m_graphNode)->syncName();
		}
		//control the bearing turn direction
		ImGui::Text(TRC(u8"旋转方向"));
		auto click_left = false, click_right = false;
		click_left = ImGui::RadioButton(TRC(u8"左"), !m_isFlipped);ImGui::SameLine(); click_right = ImGui::RadioButton(TRC(u8"右"), m_isFlipped);
		
		if(click_left) 
		{
			m_isFlipped = false;
		}
		if(click_right) 
		{
			m_isFlipped = true;
		}
		if(click_left || click_right) 
		{
			updateFlipped();        
		}
		
		auto click_steer = false, click_bear = false;
		ImGui::Text(TRC(u8"是否为舵机"));
		click_steer = ImGui::RadioButton(TRC(u8"舵机"), getIsSteering());ImGui::SameLine(); click_bear = ImGui::RadioButton(TRC(u8"普通轴承"), !getIsSteering());
		
		if(click_steer || click_bear)
		{
			setIsSteering(click_steer);
		}
		ImGui::Text(TRC(u8"是否开启角度限制"));
		bool isCurrAngleLimit;
		float angle_low;
		float angle_high;
		getAngleLimit(isCurrAngleLimit, angle_low, angle_high);
		auto click_angle_yes = false, click_angle_no = false;
		click_angle_yes = ImGui::RadioButton(TRC(u8"开启"), isCurrAngleLimit);ImGui::SameLine(); click_angle_no = ImGui::RadioButton(TRC(u8"关闭"), !isCurrAngleLimit);
		if(isCurrAngleLimit)
		{
			bool isInput = false;
			ImGui::PushItemWidth(80);
			isInput |= ImGui::InputFloat(TRC(u8"最小角度"), &angle_low);
			isInput |= ImGui::InputFloat(TRC(u8"最大角度"), &angle_high);
			ImGui::PopItemWidth();
			if(isInput)
			{
				setAngleLimit(true, angle_low, angle_high);
			}
		}
		if(click_angle_yes || click_angle_no)
		{
			setAngleLimit(click_angle_yes, angle_low, angle_high);
		}

	}
	static Matrix44 groupMatNode(vec3 pos, vec3 normal, vec3 up, Matrix44 reservedMat)
	{
	Matrix44 mat;
	auto data = mat.data();
	vec3 right = vec3::CrossProduct(normal * -1, up);
	data[0] = right.x;
	data[1] = right.y;
	data[2] = right.z;
	data[3] = 0.0;

	data[4] = up.x;
	data[5] = up.y;
	data[6] = up.z;
	data[7] = 0.0;

	data[8] = normal.x;
	data[9] = normal.y;
	data[10] = normal.z;
	data[11] = 0.0;

	data[12] = pos.x;
	data[13] = pos.y;
	data[14] = pos.z;
	data[15] = 1.0;

	return reservedMat * mat;
	}
	bool BearPart::isNeedDrawInspect()
	{
		return true;
	}

	void BearPart::updateConstraintState()
	{

		if(m_constrain)
		{
			if(m_isEnablePhysics)
			{
				PhysicsMgr::shared()->removeConstraint(m_constrain);
			}
			delete m_constrain;
		}
		m_constrain = nullptr;
		auto attachA = m_a;
		auto attachB = m_b;
		if (attachA && attachB) 
		{
			auto partA = attachA->m_parent;
			auto partB = attachB->m_parent;

			vec3 worldPosA, worldNormalA, worldUpA;
			attachA->getAttachmentInfoWorld(worldPosA, worldNormalA, worldUpA);
			vec3 worldPosB, worldNormalB, worldUpB;
			attachB->getAttachmentInfoWorld(worldPosB, worldNormalB, worldUpB);
			vec3 hingeDir = (worldPosB - worldPosA).normalized();
			vec3 center = (worldPosB + worldPosA) * 0.5f;
			//same point's transform in A Island local and B Island local, and it alway use Z axis!!!! and it is a same
			Matrix44 frameInA;
			frameInA = setUpFrameFromZ(worldPosA, hingeDir, partA->m_parent->m_node->getTransform().inverted());
			Matrix44 frameInB;
			frameInB = setUpFrameFromZ(worldPosA, hingeDir, partB->m_parent->m_node->getTransform().inverted());
			auto constrain = PhysicsMgr::shared()->createHingeConstraint(partA->m_parent->m_rigid, partB->m_parent->m_rigid, frameInA, frameInB);
			m_constrain = constrain;
		}
		if(m_isEnablePhysics)
		{
			if(m_constrain)
			{
				PhysicsMgr::shared()->addConstraint(m_constrain, true);
			}
		}
	}

	void BearPart::toggle(int signal)
	{
		float speedResult = 10.f;
		if(getIsSteering())
		{
			if(signal!= 0)
			{
				enableAngularMotor(true, speedResult * signal, 50);
			}else
			{
				enableAngularMotor(true, 0, 10000000.0f);
			}
		}else
		{
			if(signal!= 0)
			{
				enableAngularMotor(true, speedResult * signal, 50);
			}else
			{
				enableAngularMotor(false, speedResult, 50);
			}
		}
	}

	void BearPart::onUpdate(float dt)
	{
		//if(m_constrain)
		//{
		//	tlog("the angle, %f", m_constrain->getHingeAngle());
		//}
	}

	void BearPart::AddOnVehicle(Vehicle* vehicle)
	{
		m_graphNode = new BearingPartNode(this);
		vehicle->getEditor()->addNode(m_graphNode);
	}

	void BearPart::enablePhysics(bool isEnable)
{
	GameConstraint::enablePhysics(isEnable);
	if(isEnable) 
	{
		if(m_constrain)
		{
			delete m_constrain;
			m_constrain = nullptr;
		}
		if(!m_constrain) 
		{
			auto attachA = m_a;
			auto attachB = m_b;
			if (attachA && attachB) 
			{
				// createConstraintImp();
				updateConstraintState();
				// PhysicsMgr::shared()->addConstraint(m_constrain, true);
			}
		}
		else 
		{
			tlog("before add Constraint 1 the Hinge Angle is %f",m_constrain->getHingeAngle());
			PhysicsMgr::shared()->addConstraint(m_constrain, true);
		}
		if(m_isSteering)
		{
			m_constrain->enableAngularMotor(true, 0.0f, 10000.0f);
		}

		if(m_isAngleLimit)
		{
			m_constrain->setLimit(m_angleLimitLow* (TbaseMath::PI_OVER_180), m_angleLimitHigh * (TbaseMath::PI_OVER_180));
		}
	}
	else 
	{
		if(m_constrain) 
		{
			tlog("remove 1 the Hinge Angle is %f",m_constrain->getHingeAngle());
			PhysicsMgr::shared()->removeConstraint(m_constrain);
			tlog("remove 2 the Hinge Angle is %f",m_constrain->getHingeAngle());
		}
	}
}

void BearPart::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
	GameConstraint::dump(partData, allocator);
	partData.AddMember("Type", "Bearing", allocator);
	
	partData.AddMember("from", std::string(m_b->getGUID()), allocator);

	partData.AddMember("isSteering", getIsSteering(), allocator);

	bool isLimit;
	float low,high;
	getAngleLimit(isLimit, low, high);
	partData.AddMember("isAngleLimit", isLimit, allocator);
	partData.AddMember("AngleLimitLow", low, allocator);
	partData.AddMember("AngleLimitHigh", high, allocator);
	if(m_a)
	{
		partData.AddMember("to", std::string(m_a->getGUID()), allocator);
	}
	dumpAttach(partData, allocator);
	partData.AddMember("UID", std::string(getGUID()), allocator);
	partData.AddMember("isFlipped", m_isFlipped, allocator);
}

Attachment* BearPart::getFirstAttachment()
{
	return m_attachment[0];
}

Attachment* BearPart::getAttachment(int index)
{
	return m_attachment[index];
}
}

