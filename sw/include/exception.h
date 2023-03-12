/*
Copyright (C) 2022  Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef EXCEPTION_H
#define EXCEPTION_H

/**
@brief Null pointer exception
The definition of this function is application specific
*/
void throw_nullptr_error() __attribute__ ((noreturn, noinline));

/**
@brief Length error exception
The definition of this function is application specific
*/
void throw_length_error() __attribute__ ((noreturn, noinline));

/**
@brief Out of range exception
The definition of this function is application specific
*/
void throw_out_of_range() __attribute__ ((noreturn, noinline));

/**
@brief Bad alloc exception
The definition of this function is application specific
*/
void throw_bad_alloc() __attribute__ ((noreturn, noinline));

#endif