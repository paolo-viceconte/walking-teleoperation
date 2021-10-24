/**
 * @file HapticGloveModule.hpp
 * @authors Kourosh Darvish <kourosh.darvish@iit.it>
 * @copyright 2020 iCub Facility - Istituto Italiano di Tecnologia
 *            Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * @date 2020
 */

#ifndef HAPTIC_GLOVE_MODULE_HPP
#define HAPTIC_GLOVE_MODULE_HPP

#define _USE_MATH_DEFINES
#include <cmath>

// std
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

// YARP
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>
#include <yarp/sig/Vector.h>

#include <GloveControlHelper.hpp>
#include <RobotController_hapticGlove.hpp>

#include <Retargeting.hpp>
#include <iCub/ctrl/minJerkCtrl.h>

/**
 * OculusModule is the main core of the Oculus application. It is goal is to evaluate retrieve the
 * Oculus readouts, send the desired pose of the hands to the walking application, move the robot
 * fingers and move the robot head
 */

class HapticGloveModule : public yarp::os::RFModule
{
private:
    double m_dT; /**< Module period. */
    std::string m_robot; /**< robot name. */
    bool m_getHumanMotionRange; /**< get the motion range of the human joints*/

    yarp::sig::Vector m_icubLeftFingerAxisValueReference, m_icubLeftFingerAxisValueFeedback;
    yarp::sig::Vector m_icubLeftFingerAxisVelocityReference, m_icubLeftFingerAxisVelocityFeedback;
    std::vector<double> m_icubLeftFingerJointsReference, m_icubLeftFingerJointsFeedback;

    yarp::sig::Vector m_icubRightFingerAxisValueReference, m_icubRightFingerAxisValueFeedback;
    yarp::sig::Vector m_icubRightFingerAxisVelocityReference, m_icubRightFingerAxisVelocityFeedback;
    std::vector<double> m_icubRightFingerJointsReference, m_icubRightFingerJointsFeedback;

    std::vector<double> m_gloveRightBuzzMotorReference, m_gloveLeftBuzzMotorReference;
    std::vector<double> m_gloveRightForceFeedbackReference, m_gloveLeftForceFeedbackReference;

    double m_timePreparation, m_timeConfigurationEnding, m_timeNow;

    std::unique_ptr<iCub::ctrl::minJerkTrajGen> m_rightAxisValueErrorSmoother{nullptr},
        m_leftAxisValueErrorSmoother{nullptr};
    std::unique_ptr<iCub::ctrl::minJerkTrajGen> m_rightAxisVelocityErrorSmoother{nullptr},
        m_leftAxisVelocityErrorSmoother{nullptr};

    std::vector<double> m_icubRightFingerAxisValueError, m_icubRightFingerAxisValueErrorSmoothed;
    std::vector<double> m_icubLeftFingerAxisValueError, m_icubLeftFingerAxisValueErrorSmoothed;

    std::vector<double> m_icubRightFingerAxisVelocityError,
        m_icubRightFingerAxisVelocityErrorSmoothed;
    std::vector<double> m_icubLeftFingerAxisVelocityError,
        m_icubLeftFingerAxisVelocityErrorSmoothed;

    yarp::sig::Vector m_leftTotalGain, m_rightTotalGain;
    yarp::sig::Vector m_leftVelocityGain, m_rightVelocityGain;
    yarp::sig::Vector m_leftBuzzMotorsGain, m_rightBuzzMotorsGain;

    double m_velocity_threshold_transient;
    double m_Value_error_threshold_transient;

    /** Haptic Glove Finite state machine */
    enum class HapticGloveFSM
    {
        Configured,
        Running,
        InPreparation
    };

    class Logger;

    std::unique_ptr<Logger> m_loggerLeftHand, m_loggerRightHand;

    HapticGloveFSM m_state; /**< State of the HapticGloveFSM */

    bool m_moveRobot; /**< the option to give the user the possibility to do not move the robot
                         (default :: true)*/

    std::unique_ptr<RobotController> m_robotLeftHand; /**< Pointer to the left
                                                              finger retargeting object. */
    std::unique_ptr<RobotController> m_robotRightHand; /**< Pointer to the right
                                                               finger retargeting object. */

    std::unique_ptr<HapticGlove::GloveControlHelper> m_gloveRightHand; /**< Pointer to the right
                                                               hand glove object. */
    std::unique_ptr<HapticGlove::GloveControlHelper> m_gloveLeftHand; /**< Pointer to the left
                                                               hand glove object. */

    std::unique_ptr<HapticGlove::Retargeting> m_retargetingLeftHand;

    std::unique_ptr<HapticGlove::Retargeting> m_retargetingRightHand;

    bool m_enableLogger; /**< log the data (if ON) */
    bool m_useLeftHand,
        m_useRightHand; /**< use the specided hand if the flag is ON (default value is ON)*/
    double m_calibrationTimePeriod; /**< calibration time period [sec] */

    /**
     * Get all the feedback signal from the interfaces
     * @return true in case of success and false otherwise.
     */
    bool getFeedbacks();

    /**
     * Evaluate the desired robot fingers reference (velocity or position)
     * @return if the evaluation is done successfully
     */
    bool evaluateRobotFingersReferences();

public:
    HapticGloveModule();
    ~HapticGloveModule();
    /**
     * Get the period of the RFModule.
     * @return the period of the module.
     */
    double getPeriod() final;

    /**
     * Main function of the RFModule.
     * @return true in case of success and false otherwise.
     */
    bool updateModule() final;

    /**
     * Configure the RFModule.
     * @param rf is the reference to a resource finder object
     * @return true in case of success and false otherwise.
     */
    bool configure(yarp::os::ResourceFinder& rf) final;

    /**
     * Close the RFModule.
     * @return true in case of success and false otherwise.
     */
    bool close() final;
};

inline std::string getTimeDateMatExtension()
{
    // this code snippet is taken from
    // https://stackoverflow.com/questions/17223096/outputting-date-and-time-in-c-using-stdchrono
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char timedate[30];

    std::strftime(&timedate[0], 30, "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
    std::string timeDateStr = timedate;
    timeDateStr.shrink_to_fit();
    return timeDateStr;
}
#endif
