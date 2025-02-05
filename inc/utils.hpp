/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:06:56 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/05 11:08:36 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "sstream"

template<typename T>
std::string to_string(T element)
{
	std::stringstream ss;
	ss << element;
	return ss.str();
}
#endif
