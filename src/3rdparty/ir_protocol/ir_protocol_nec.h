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
 *          Frequency:          36 kHz
 * 
 *          Preamble:           9000us Pulse
 *                              4500us Pause
 * 
 *          Logical-One:        560us Pulse
 *                              1690 s Pause
 * 
 *          Logical Zero:       560us Pulse
 *                              560us Pause
 * 
 *          Stop-Bit:           560us Pulse
 * 
 *          Usage:
 * 
 *              1. Set timers
 *              
 *                  ir_protocol_nec_set_timer()
 *                      
 *              2. Start transmission
 *                          
 *                  ir_protocol_nec_transmit()
 *                      
 *              3. Wait for completion
 * 
 *                  ir_protocol_nec_is_busy()
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_ir_protocol_nec_
#define _H_ir_protocol_nec_

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void ir_protocol_nec_init(void);

// --------------------------------------------------------------------------------

#endif // _H_ir_protocol_nec_

// --------------------------------------------------------------------------------
