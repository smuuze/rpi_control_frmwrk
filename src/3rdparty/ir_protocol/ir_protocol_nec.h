/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file   ir_protocol_nec.h
 * @author Sebastian Lesse
 * @date   2021 / 12 / 28
 * @brief   Module to generate a NEC ir-command
 * 
 *  Frequency:  36 kHz
 * 
 *  Preamble:       9000us Pulse
 *                  4500us Pause
 * 
 *  Logical-One:    560us Pulse
 *                  1690 s Pause
 * 
 *  Logical Zero:   560us Pulse
 *                  560us Pause
 * 
 *  Stop-Bit:       560us Pulse
 * 
 *  Usage:
 * 
 *  The IR-protocl module uses the IR-Protocol-Interface.
 *  By calling ir_protocol_sony_init() a new ir-protocol will
 *  be registered. The ID of this procotol is set to 0x03
 * 
 *  @see 3rdparty/ir_protocol/ir_protocol_interface.h
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_ir_protocol_nec_
#define _H_ir_protocol_nec_

// --------------------------------------------------------------------------------

/**
 * @brief  Initialized the NEC ir-protocol.
 * Registers a new ir-protocol to the ir-protocol-interface
 * 
 */
void ir_protocol_nec_init(void);

// --------------------------------------------------------------------------------

#endif // _H_ir_protocol_nec_

// --------------------------------------------------------------------------------
