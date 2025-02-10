/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:38:35 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/10 15:36:16 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHESSBOT_HPP
# define CHESSBOT_HPP

# define TOPBORDER 4
# define SIDEBORDER 5
# define CASEWIDTH 9
# define CASEHEIGHT 5

#include <string>

class ChessBot
{
	public:
		ChessBot();
		ChessBot(const ChessBot &copy);
		ChessBot &operator=(const ChessBot &other);
		~ChessBot();

		void 	PrintBoard(std::string &response);
		void	MovePiece(std::string movement);
		void 	ClearCase(std::string pos);
		void 	PutPiece(std::string piece, std::string pos);
		void	GetPos(std::string pos, int &i, int &j);
	private:
		std::string	_fen;
		std::string *_board;
};

#endif