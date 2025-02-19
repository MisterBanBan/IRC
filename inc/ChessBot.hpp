/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:38:35 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/17 19:19:57 by arvoyer          ###   ########.fr       */
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
		~ChessBot();

		void	 	PrintBoard(std::string &response);
		void		MovePiece(std::string movement);
		void 		ClearCase(int i, int j);
		void 		PutPiece(std::string piece, int j, int i);
		std::string	MakeABotMove();
		void		MakeAPlayerMove(std::string movement);
		void		ParseBotMove(std::string movement, std::string &response);
		void		reset();
		
		int			GetPosI(char pos);
		int			GetPosJ(char pos);
		std::string	GetPieceFile(char piece);
		int			GetFenPos(std::string pos);
		void		MakeTableFromFen();
		void		MakeFenFromTable();
		bool		There2King();
	private:
		std::vector<std::string>	_fen;
		std::vector<std::string>	_board;
		std::vector<std::string>	_table;
};

#endif