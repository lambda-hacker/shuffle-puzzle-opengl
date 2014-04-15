#ifndef _GRID_HPP_
#define _GRID_HPP_

#include <vector>
#include <ctime>
#include "vector2.hpp"

using std::vector;

namespace smadani {

	class Grid;
	
	class Piece 
	{
	public:
		Piece();
		Piece(size_t r, size_t c, size_t nrows, size_t ncols);
		Piece(Piece const& rhs);
		~Piece();

		size_t row() const;
		size_t column() const;
		Piece& row(size_t r);
		Piece& column(size_t c);

		Piece& operator=(Piece const& rhs);
		Vector2 const& tc1() const;
		Vector2 const& tc2() const;
		Vector2 const& tc3() const;
		Vector2 const& tc4() const;

	private:
		size_t r, c;			//Actual row, column position of the piece
		Vector2 t1, t2, t3, t4; //Texture coordinates mapping the Image[Texture]
	};

	Piece::Piece(size_t r, size_t c, size_t nrows, size_t ncols): r(r), c(c)
	{
	
		// std::cout << r << " " <<  c << std::endl;

		//Offset for texture coordinates of the pieces
		GLfloat tcord_x_offset = (1 / (GLfloat)ncols);
		GLfloat tcord_y_offset = (1 / (GLfloat)nrows);


	   //Calculating Texture Coordinates
		t1.x = c  *  tcord_x_offset;
		t1.y = r  *  tcord_y_offset; 

		t2.x = (c + 1) * tcord_x_offset;
		t2.y = r  *  tcord_y_offset;

		t3.x = (c + 1) * tcord_x_offset;
		t3.y = (r + 1) * tcord_y_offset;

		t4.x = c * tcord_x_offset;
		t4.y = (r + 1) * tcord_y_offset;	


		std::cout << t1.x << " " << t1.y << ", ";
		std::cout << t2.x << " " << t2.y << ", ";
		std::cout << t3.x << " " << t3.y << ", ";
		std::cout << t4.x << " " << t4.y << std::endl;
	}
	Piece::Piece() { }
	Piece::Piece(Piece const& rhs) 
	{
		*this = rhs;
	}
	
	Piece::~Piece()  { }
	
	size_t Piece::row()    const { return r; }
	size_t Piece::column() const { return c; }
	Piece& Piece::row(size_t nr)    { r = nr; return *this; }
	Piece& Piece::column(size_t nc) { c = nc; return *this; }
	Piece& Piece::operator=(Piece const& rhs)
	{
		r = rhs.r;
		c = rhs.c;
		t1 = rhs.t1;
		t2 = rhs.t2;
		t3 = rhs.t3;
		t4 = rhs.t4;
		return *this;
	}
	Vector2 const& Piece::tc1() const { return t1; }
	Vector2 const& Piece::tc2() const { return t2; }
	Vector2 const& Piece::tc3() const { return t3; }
	Vector2 const& Piece::tc4() const { return t4; }

	void swap_piece(Piece& p1, Piece& p2)
	{
		Piece t = p1;
		p1 = p2;
		p2 = t;
	}


	class Grid {
	public:
		Grid(size_t r = 4, size_t c = 4);
		~Grid();
		size_t num_rows()	 const;
		size_t num_columns() const;
		size_t e_r() const;
		size_t e_c() const;

		Piece const& grid_piece(size_t r, size_t c) const;
		bool is_all_pieces_in_place() const;
		bool is_active() const;

	private:
		size_t rows, columns;
		Piece** grid;		
		bool grid_active;
		size_t empty_r, empty_c;

		friend class GridController;
		friend class Shuffler;
	};


	Grid::Grid(size_t r, size_t c) : rows(r), columns(c)
	{
		grid = new Piece*[rows];
		for (size_t i = 0; i < rows; i++)
		{
			grid[i] = new Piece[columns];
		}


		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < columns; j++)
			{
				grid[i][j] = Piece(i, j, rows, columns);
			}
		}

		empty_r = rows - 1;
		empty_c = columns - 1;

		grid_active = true;
	}
	Grid::~Grid()
	{
		for (size_t i = 0; i < rows; i++)
			if (grid[i] != NULL) delete [] grid[i];
		delete [] grid;
	}
	inline size_t Grid::num_rows()    const { return rows;    }
	inline size_t Grid::num_columns() const { return columns; }
	inline size_t Grid::e_r() const { return empty_r; }
	inline size_t Grid::e_c() const { return empty_c; }

	Piece const& Grid::grid_piece(size_t r, size_t c) const
	{
		return grid[r][c];
	}
	bool Grid::is_active() const { return grid_active; }
	bool Grid::is_all_pieces_in_place() const 
	{
		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < columns; j++)
			{
				size_t pr = grid[i][j].row();
				size_t pc = grid[i][j].column();
				if ( i != pr ) return false;
				if ( j != pc ) return false;
			}
		}
		return true;
	}


	
	class Shuffler {
	public:
		Shuffler(Grid& grid);
		~Shuffler();
		void shuffle();
		void randomize();
	private:
		Grid* grid_to_shuffle;
	};

	Shuffler::Shuffler(Grid& grid)
	{
		grid_to_shuffle = &grid;
	}
	Shuffler::~Shuffler() { }

	void Shuffler::shuffle()
	{
		size_t r = grid_to_shuffle->num_rows();
		size_t c = grid_to_shuffle->num_columns();

		for (size_t i = 0; i < r * c; i++)
		{
			randomize();
			size_t x1 = rand() % r;
			size_t y1 = rand() % c;

			size_t x2 = rand() % r;
			size_t y2 = rand() % c;
			
			do {
				randomize();
				x2 = rand() % r;
				y2 = rand() % c;
			} while (x1 == x2 && y1 == y2);

			swap_piece(grid_to_shuffle->grid[x1][y1], grid_to_shuffle->grid[x2][y2]);
			
			if (x1 == grid_to_shuffle->empty_r && y1 == grid_to_shuffle->empty_c)
			{
				grid_to_shuffle->empty_r = x2;
				grid_to_shuffle->empty_c = y2;
			}
			else if (x2 == grid_to_shuffle->empty_r && y2 == grid_to_shuffle->empty_c)
			{
				grid_to_shuffle->empty_r = x1;
				grid_to_shuffle->empty_c = y1;
			}
		}
	}
		
	
	void Shuffler::randomize()
	{
	}

	
	class GridController {
	public:
		GridController(Grid& grid_to_control);
		bool move(size_t r, size_t c);
		bool is_game_started() const;
		void game_reset();

	private:
		Grid* grid;
		bool game_started;

		bool can_move(size_t r, size_t c);
	};

	GridController::GridController(Grid& grid_to_control)
	{
		grid = &grid_to_control;
		game_started = false;
	}

	bool GridController::move(size_t r, size_t c)
	{
		if (can_move(r, c))
		{
			if (!game_started) game_started = true;
			swap_piece(grid->grid[r][c], grid->grid[grid->e_r()][grid->e_c()]);
						
			grid->empty_r = r;
			grid->empty_c = c;
			//std::cout << " Move " << std::endl;
			return true;
		}

		return false;
	}

	bool GridController::can_move(size_t ri, size_t cj)
	{
		if ((abs(ri - grid->e_r()) + abs(cj - grid->e_c()) == 1) && grid->grid_active)
			return true;
		return false;
	}

	bool GridController::is_game_started() const
	{
		return game_started;
	}

	void GridController::game_reset()
	{
		game_started = false;
	}


	class Timer {
	public:
		Timer();
		void start();
		void stop();
		void reset();
		bool is_started();
		time_t get_time();
	private:
		bool started;
		time_t m_time;
		clock_t m_clock;
	};

	Timer::Timer(): started(false)
	{
	}

	void Timer::start()
	{
		m_clock = clock();
		started = true;
	}

	void Timer::stop()
	{
		started = false;
	}

	void Timer::reset()
	{
		m_clock = 0;
		m_time = 0;
		started = false;
	}

	bool Timer::is_started()
	{
		return started;
	}
	time_t Timer::get_time()
	{
		if (started) {
			m_time = clock() - m_clock;
		}
		return m_time;
	}

	class Scorer {
	public:
		Scorer();
		void increment_steps();
		size_t get_num_steps() const;
		void reset();

	private:
		size_t num_steps;
		size_t score;
	};

	Scorer::Scorer():num_steps(0), score(0)
	{
	}
	void Scorer::increment_steps()
	{
		++num_steps;
	}
	size_t Scorer::get_num_steps() const
	{
		return num_steps;
	}
	void Scorer::reset()
	{
		num_steps = 0;
	}
}

#endif