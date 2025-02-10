/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:38:35 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/10 17:18:02 by arvoyer          ###   ########.fr       */
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
		void 	ClearCase(int i, int j);
		void 	PutPiece(std::string piece, int j, int i);
		
		int			GetPosI(char pos);
		int			GetPosJ(char pos);
		std::string	GetPieceFile(char piece);
		int			GetFenPos(std::string pos);
	private:
		std::string	_fen;
		std::string *_board;
};

#endif