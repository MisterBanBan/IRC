/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:38:35 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/12 13:51:53 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHESSBOT_HPP
# define CHESSBOT_HPP

# define TOPBORDER 4
# define SIDEBORDER 5
# define CASEWIDTH 9
# define CASEHEIGHT 5

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>

class ChessBot
{
	public:
		ChessBot();
		//ChessBot(const ChessBot &copy);
	//	ChessBot &operator=(const ChessBot &other);
		~ChessBot();

		void	 	PrintBoard(std::string &response);
		void		MovePiece(std::string movement);
		void 		ClearCase(int i, int j);
		void 		PutPiece(std::string piece, int j, int i);
		std::string	MakeABotMove();
		void		ParseBotMove(std::string movement, std::string &response);
		
		int			GetPosI(char pos);
		int			GetPosJ(char pos);
		std::string	GetPieceFile(char piece);
		int			GetFenPos(std::string pos);
		void		MakeTableFromFen();
		void		MakeFenFromTable();
	private:
		std::string	_fen;
		std::vector<std::string> _board;
		std::vector<std::string> _table;
};

#endif