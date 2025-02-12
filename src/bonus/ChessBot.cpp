/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 14:41:50 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/12 15:09:14 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChessBot.hpp"

std::string CallAPI(const char* cmd);

ChessBot::ChessBot()
{
	_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	
	std::ifstream	in("ChessBoard/ChessBoard.chess");
	if (in.is_open() == false)
	{
		_fen = "\0";
		return ;
	}
	
	for (int j = 0; j < 8; j++)
		this->_table.push_back("00000000");
	this->MakeTableFromFen();
	
	std::string line;

	while (in.eof() == false)
	{
		std::getline(in, line);
		_board.push_back(line + "\n");
	}
}

ChessBot::~ChessBot() {}

void	ChessBot::PrintBoard(std::string &response)
{
	if (_fen == "\0")
	{
		response += "This isn't a valid move\n";
		return ;
	}
	for (std::vector<std::string>::iterator i = _board.begin(); i != _board.end(); i++)
	{
		response += *i;
	}
	response += "\n";
}

void ChessBot::ClearCase(int i, int j)
{
	int iEnd = i + CASEWIDTH - 1;
	int jEnd = j + CASEHEIGHT - 1;

	char ClearChar = _board[j][i];
	
	for (; j <= jEnd; j++)
	{
		for (; i < iEnd; i++)
		{
			_board[j][i] = ClearChar;
		}
		i -= CASEWIDTH - 1;
	}
}

void	ChessBot::PutPiece(std::string piece, int i, int j)
{
	std::ifstream	in(piece.c_str());
	if (in.is_open() == false)
	{
		_fen = "\0";
		return ;
	}

	i--;
	int starti = i;

	std::string line;

	while (in.eof() == false)
	{
		std::getline(in, line);
		for (int i2 = 0; line[i2]; i2++)
		{
			i++;
			if (!line[i2])
				break ;
			if (line[i2] == ' ' || line[i2] == '#')
				continue ;
			_board[j][i] = line[i2];
		}
		i = starti;
		j++;
	}
}

void	ChessBot::MovePiece(std::string movement)
{
	int	i = this->GetFenPos(movement);
	char piece = _fen[i];
	
	if (!std::isalpha(piece))
	{
		_fen = "\0";
		return ;
	}
	
	this->ClearCase(this->GetPosI(movement[0]), this->GetPosJ(movement[1]));
	std::string piece_file = this->GetPieceFile(piece);
	this->ClearCase(this->GetPosI(movement[2]), this->GetPosJ(movement[3]));
	this->PutPiece(piece_file, this->GetPosI(movement[2]), this->GetPosJ(movement[3]));
	
	_table[8 - (movement[1] - '0')][movement[0] - 'a'] = '0';

	movement.erase(0, 2);
		
	_table[8 - (movement[1] - '0')][movement[0] - 'a'] = piece;
	
	this->MakeFenFromTable();
}

std::string	ChessBot::MakeABotMove()
{
    std::string url = "https://chess-stockfish-16-api.p.rapidapi.com/chess/api";
    std::string apiKey = "5a6aa54142msh45f81e3897d6fcap1aa1bejsn148d461e8c21";
    std::string apiHost = "chess-stockfish-16-api.p.rapidapi.com";

    std::string command = "curl -s -X POST \"" + url + "\" "
                          "-H \"x-rapidapi-key: " + apiKey + "\" "
                          "-H \"x-rapidapi-host: " + apiHost + "\" "
                          "-H \"Content-Type: application/x-www-form-urlencoded\" "
                          "-d \"fen=" + _fen + "\"";

    try
	{
        std::string response = CallAPI(command.c_str());
        return (response);
    }
	catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
	
	std::string response = "\0";
	return (response);
}

void	ChessBot::ParseBotMove(std::string movement, std::string &response)
{
	if (movement == "\0" || _fen == "\0")
		return ;
	size_t	i = movement.find("\"bestmove\":");
	std::string move = movement.substr(i + 12, 4);
	
	this->MovePiece(move);
	response += move + "\n";
}