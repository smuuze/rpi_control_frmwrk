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
 * @file    initialization.h
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 05
 * @brief   Initialization of everything
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_initialization_
#define _H_initialization_

// --------------------------------------------------------------------------------

/**
 * @brief Performs a whole initialization
 * of all system- and program components.
 * Must be called before any other operation takes place.
 * 
 */
void initialization(void);

// --------------------------------------------------------------------------------

/**
 * @brief Performs whole de-initialization
 * of all system- and program components.
 * After calling nothing is in a valid/usable state
 * 
 */
void deinitialization(void);

// --------------------------------------------------------------------------------

#endif // _H_initialization_

// --------------------------------------------------------------------------------

