/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * Reproduction in whole or in part is prohibited without the written consent*
 * of the copyright owner. Austriamicrosystems reserves the right to make    *
 * changes without notice at any time. The software is provided as is and    *
 * Austriamicrosystems makes no warranty, expressed, implied or statutory,   *
 * including but not limited to any implied warranty of merchantability or   *
 * fitness for any particular purpose, or that the use will not infringe any *
 * third party patent, copyright or trademark. Austriamicrosystems should    *
 * not be liable for any loss or damage arising from its use.                *
 *****************************************************************************
 */
 
/*
 * PROJECT: AS3911 firmware
 * $Revision: $
 * LANGUAGE: ANSI C
 */
  
/*! \file emv_main.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV terminal application.
 */

#ifndef EMV_MAIN_H
#define EMV_MAIN_H

#if defined(EMV_AS3911)
/*! \defgroup emvMain EMV Layer 4 and Example Terminal Application
 *****************************************************************************
 * \ingroup emv
 *
 * \brief EMV Layer 4 data transmission and example implementation of an EMV
 * contactless terminal main loop.
 *
 * This module contains the EMV layer 4 stack, an exmaple implementation
 * of the EMV terminal main loop, and some examples for EMV terminal
 * applications.
 *
 * \section emvMain_sec1 EMV Layer 4
 *
 * The layer 4 implementation of the EMV stack consists of only two functions.
 *
 * One function to init the layer 4: emvInitLayer4(). This must be used after
 * successfull activation of a PICC to initialize the ISO14443-4 half duplex
 * protocol stack.
 *
 * And, one function to communicate with a card on layer 4 (in the context
 * of APDUs): emvTransceiveApdu(). This function deals with all the timeout,
 * error handling, and retransmission requirements of the EMV contactless
 * communication protocol and thus provides a transparent way to transmit
 * an APDU to the RFID card and receive any RPDU send by the card in response.
 *
 * \section emvMain_sec2 Example EMV Terminal Application Main Loop
 *
 * The example terminal application main loop is intended as a blue print which
 * shows how different parts of the EMV software stack should be used together
 * to implement an EMV standard compliant terminal application.
 *
 * The function emvStartTerminalApplication() will start an EMV terminal
 * main loop as defined in the EMV standard. This main loop pools for cards
 * until a card can be successfully activated and then transfers control
 * to the specified application callback to perform the payment application.
 * After the application callback returns a card removal is executed. Once
 * the RFID card has been removed from the field the terminal main loop
 * starts over again and polls for a new card.
 *
 * This exapmle terminal main loop continues to run until a call (from
 * another thread) is made to emvStopTerminalApplication(). The emv layer
 * does not check for terminal application stop requests while an RFID data
 * transmission is in progress. Therefore it can take up to 5 seconds until
 * a stop requests leads to the termination of the terminal main loop (about
 * 5 seconds is the maximum allowed timeout for a single block transmission
 * on layer 4).
 *
 * \section emvMain_sec3 Example Terminal Application Callbacks
 *
 * Two example application callbacks for the terminal application main loop
 * are provided. The prevalidation test application
 * emvPrevalidationApplication() and the digital test application
 * emvDigitalApplication(). These two implement the application logic specified
 * in the requirements for the prevaliation test and digital test of the
 * EMV L1 certification.
 *
 * \section emvMain_sec4 Implementation Overview of the Terminal Application Main Loop
 *
 * The polling loop of the terminal application main loop is implemented in
 * emvPoll(). A call to this function blocks until a card is found or a
 * stop of the terminal application main loop is requested.
 *
 * If the presence of a card is indicated by the return value of emvPoll(),
 * then emvCollisionDetection() is used to perform an EMV compliant collision
 * detection for both type A and type B cards. After successfull collision
 * detection the ::EmvPicc_t paramter of emvCollisionDetection() references the
 * singulated PICC.
 *
 * This ::EmvPicc_t value can then be used to activate (emvActivate()) and
 * initialize (emvInitLayer4()) the layer 4 protocol, perform data transmission
 * (emvTransceiveApdu()), and finally to check for the correct removal of the
 * card form the field (emvRemove()).
 *****************************************************************************
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/
typedef struct
{
	volatile char polldeflg;   //poll 
	volatile char acolldeflg;  //type a Acollision  type b poll
	volatile char activedeflg; //type a/b active
	volatile char apdudeflg;   //type a/b apdu protocol

	//volatile char emvproflg0;   //test flg
	//volatile char emvproflg1;  
	//volatile char emvproflg2; 
	//volatile char emvproflg3; 
} RFID_EMV_DEBUGFLG;

extern RFID_EMV_DEBUGFLG gemvdebugflg; 

typedef struct
{
    uchar cardtype;  //0-no card or no active
                     //1-type a   2-type b
	uchar ATQA[2];   //type a
    uchar uidLength; /*!< Length of the UID/PUPI in bytes. */
    uchar uid[10]; /*!< UID or PUPI of the PICC. */
    uchar ATS[20]; /*!< UID or PUPI of the PICC. */
	uchar ATQB[13];  //type b
    uchar ATTRIB[9];    //ats to ATTRIB 
    uchar ATOATTRIB;    //ats to ATTRIB 
} RFID_EMV_CARDINFO;

extern RFID_EMV_CARDINFO gemvcardinfo;
/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup emvMain
 *****************************************************************************
 * \brief Start the EMV terminal application.
 *
 * Start the EMV terminal application. The terminal main loop polls
 * for cards until a single card is detected and successfully activated. Then
 * the application callback is called to perform any higher layer
 * operations. After the callback returns either a field reset or card removal
 * is performed, depending on the error status returned by the application
 * callback. Then the cycle starts again with polling.
 *
 * \note You need to call emvStopTerminalApplication() to stop the terminal
 * application from outside. Otherwise this function call blocks the
 * application callback requests a stop.
 *
 * \param[in] application Higher layer application call back.
 *
 * \return EMV_ERR_STOPPED: The terminal application has been stopped due to a
 * stop request.
 *****************************************************************************
 */
s16 emvStartTerminalApplication(s16 (*application)(void));

/*! \ingroup emvMain
 *****************************************************************************
 * \brief Check whether a stop terminal application request has been received
 * from the GUI.
 *
 * \return TRUE: A stop terminal request has been received from the GUI.
 * \return FALSE: No stop terminal request has been received.
 *****************************************************************************
 */
bool_t emvStopRequestReceived();

/*! \ingroup emvMain
 *****************************************************************************
 * \brief Request that any running emv terminal application is stopped.
 *****************************************************************************
 */
void emvStopTerminalApplication();

#endif
#endif /* EMV_MAIN_H */

