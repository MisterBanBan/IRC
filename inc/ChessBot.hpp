/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:38:35 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/07 14:38:00 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHESSBOT_HPP
# define CHESSBOT_HPP

#include <string>

class ChessBot
{
	public:
		ChessBot();
		ChessBot(const ChessBot &copy);
		ChessBot &operator=(const ChessBot &other);
		~ChessBot();
	private:
		std::string	_fen;
		std::string *_board;
};

#endif