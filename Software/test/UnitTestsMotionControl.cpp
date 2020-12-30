#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "gcode.h"
#include "logging.h"
#include "machineproperties.h"
#include "motion.h"
#include "motionbuffer.h"
#include "motionctrl.h"
#include "motionperipherals.h"
#include "motionspeedprofile.h"
#include "motiontrajectory.h"

uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestMotionCtrl {
TEST_CLASS (C01_motionTrajectory) {
  public:
    TEST_METHOD (T01_Construction) {
        motionTrajectory trajectory;
        //Logger::WriteMessage("some output during testing");
    }
};

TEST_CLASS (C02_motionSpeedProfile) {
  public:
    TEST_METHOD (T01_Construction) {
        motionSpeedProfile speedProfile;
    }
};

TEST_CLASS (C03_motionPeripheral) {
  public:
    TEST_METHOD (T01_Construction) {
        motionPeripherals peripherals;
    }
};

TEST_CLASS (C04_motion) {
  public:
    TEST_METHOD (T01_Construction) {
        motion theMotion;
    }
    TEST_METHOD (T02_vOut) {
        motion theMotion;
        //        float vOut(float vIn, float aMax, float dMax, float length, motionStrategy strategy, motionCalculateDirection direction) const;
        Assert::AreEqual(30.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::maximizeSpeed, motionCalculateDirection::forward), 0.0001F);
        Assert::AreEqual(30.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::maximizeSpeed, motionCalculateDirection::reverse), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vOut(30.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::forward), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vOut(30.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::reverse), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::forward), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::reverse), 0.0001F);
    }
    TEST_METHOD (T03_vTri) {
        motion theMotion;
        //        float vTri(float vStart, float vEnd, float aMax, float dMax, float length, motionStrategy strategy) const;
        Assert::AreEqual(20.0F, theMotion.vTri(10.0F, 00.0F, 10.0F, -10.0F, 35.0F, motionStrategy::maximizeSpeed), 0.0001F);
        Assert::AreEqual(20.0F, theMotion.vTri(00.0F, 10.0F, 10.0F, -10.0F, 35.0F, motionStrategy::maximizeSpeed), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vTri(20.0F, 30.0F, 10.0F, -10.0F, 55.0F, motionStrategy::minimizeSpeed), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vTri(30.0F, 20.0F, 10.0F, -10.0F, 55.0F, motionStrategy::minimizeSpeed), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vTri(10.0F, 10.0F, 10.0F, -10.0F, 10.0F, motionStrategy::minimizeSpeed), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vTri(10.0F, 10.0F, 10.0F, -10.0F, 10.0F, motionStrategy::minimizeSpeed), 0.0001F);
    }

    TEST_METHOD (T04_Optimize) {
        motion theMotion;
    }
};

TEST_CLASS (C05_motionBuffer) {
  public:
    TEST_METHOD (T01_Construction) {
        motionBuffer theMotionBuffer;
        Assert::IsTrue(theMotionBuffer.isEmpty(), L"001");
    }

    TEST_METHOD (T02_PushAndPop) {
        motionBuffer theMotionBuffer;
        uint32_t newItemIndex = theMotionBuffer.push();
        Assert::IsTrue(newItemIndex == 0, L"002");
        Assert::IsFalse(theMotionBuffer.isEmpty(), L"003");
        Assert::IsTrue(theMotionBuffer.bufferLevel == 1, L"004");

        theMotionBuffer.pop();
        Assert::IsTrue(theMotionBuffer.isEmpty(), L"005");
    }

    TEST_METHOD (T03_DetectFull) {
        motionBuffer theMotionBuffer;
        for (uint32_t i = 0; i < theMotionBuffer.bufferLength; i++) {
            (void)theMotionBuffer.push();
        }
        Assert::IsTrue(theMotionBuffer.isFull());
        Assert::IsFalse(theMotionBuffer.isEmpty());

        for (uint32_t i = 0; i < theMotionBuffer.bufferLength; i++) {
            theMotionBuffer.pop();
        }
        Assert::IsFalse(theMotionBuffer.isFull());
        Assert::IsTrue(theMotionBuffer.isEmpty());
    }
};

TEST_CLASS (C06_motionCtrl) {
  public:
    TEST_METHOD (T01_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T002_getNextStep) {
        Logger::WriteMessage("motionBuffer empty ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        Assert::IsTrue(theMotionController.theMotionBuffer.isEmpty());
        step aStep = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T003_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = ready or stopped ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::ready;
        (void)theMotionController.theMotionBuffer.push();
        aStep = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionController.theState = motionState::stopped;
        aStep                        = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T004_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, slow movement ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::running;
        (void)theMotionController.theMotionBuffer.push();
        motion* aMotion = theMotionController.theMotionBuffer.current();
        aMotion->setForTest(0U);

        aStep = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionController.theState = motionState::stopping;
        aStep                        = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T005_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, fast movement ->  : output is steps");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::running;
        (void)theMotionController.theMotionBuffer.push();
        motion* aMotion = theMotionController.theMotionBuffer.current();
        aMotion->setForTest(1U);
        aStep = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
        theMotionController.theState = motionState::stopping;
        aStep                        = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
    }

    TEST_METHOD (T006_MotionComplete) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::running;
        (void)theMotionController.theMotionBuffer.push();
        theMotionController.theMotionBuffer.motionBuffer[0].setForTest(1U);
        (void)theMotionController.theMotionBuffer.push();
        theMotionController.theMotionBuffer.motionBuffer[1].setForTest(1U);

        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 80; i++) {
            aStep = theMotionController.nextStep();
        }
        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 40; i++) {
            aStep = theMotionController.nextStep();
        }
        Assert::IsTrue(theEventBuffer.hasEvents());
    }

    TEST_METHOD (T005_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T006_vJunction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T007_OptimizePair) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }
};

}        // namespace UnitTestMotionCtrl
