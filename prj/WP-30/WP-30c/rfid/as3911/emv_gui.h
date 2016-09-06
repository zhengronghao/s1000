/*
 *****************************************************************************
 * Copyright @ 2011                                 *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */
/*
 *      PROJECT:   AS3911 firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file emv_gui.h
 *
 *  \author Oliver Regenfelder
 *
 *  \brief EMV TTA L1 GUI commands.
 */

#ifndef EMV_GUI_H
#define EMV_GUI_H

#if defined(EMV_AS3911)
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI toggle carrier command.
 *
 * Toggles the reader field between active and inactive state
 ****************************************************************************
 */
void emvGuiToggleCarrier();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI poll command.
 *
 * Perform a single iteration of the EMV card polling loop.
 ****************************************************************************
 */
void emvGuiPoll();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI reset command.
 *
 * Perform a reader field reset.
 ****************************************************************************
 */
void emvGuiReset();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI WUPA command.
 *
 * Send a single WUPA request.
 ****************************************************************************
 */
void emvGuiWupa();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI WUPB command.
 *
 * Send a single WUPB request.
 ****************************************************************************
 */
void emvGuiWupb();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI RATS command.
 *
 * Perform a WUPA, RATS sequence.
 ****************************************************************************
 */
void emvGuiRats();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI ATTRIB command.
 *
 * Perform a WUPB, ATTRIB sequence.
 ****************************************************************************
 */
void emvGuiAttrib();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI prevalidation test command.
 *
 * Start the prevalidation test application.
 *
 * \note This function call blocks until emvGuiStop() is called.
 ****************************************************************************
 */
void emvGuiPrevalidation();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI digital test command.
 *
 * Start the digital test application.
 *
 * \note This function call blocks until emvGuiStop is called.
 ****************************************************************************
 */
void emvGuiDigital();

/*! \ingroup usb
 ****************************************************************************
 * \brief EMV TTA GUI digital or prevalidation test stop command.
 *
 * Stop any running digital or prevalidation test application.
 ****************************************************************************
 */
void emvGuiStop();

uchar s_emvGuiWupa(uchar * pucResp);
void s_emvGuiHaltA(void);
uchar s_emvGuiWupb(uchar * pucOutLen, uchar * pucOut);
#endif
#endif /* EMV_GUI_H */

