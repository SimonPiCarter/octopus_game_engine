#include <gtest/gtest.h>

struct Position
{
	long x = 0;
	long y = 0;
};

class CacheTest : public testing::Test
{
protected:
    static void SetUpTestSuite()
	{
		size_t size_l = 5000;
		_continuous.reserve(size_l);
		_random.reserve(size_l);
		std::vector<Position *> temp_l;
		temp_l.reserve(size_l);
		for(size_t i = 0 ; i < size_l ; ++ i)
		{
			_continuous.push_back(Position());
			temp_l.push_back(new Position());
			_random.push_back(nullptr);
		}
		size_t batch_l = 100;
		for(size_t i = 0 ; i < size_l/batch_l ; ++ i)
		{
			for(size_t j = 0 ; j < batch_l ; ++ j)
			{
				std::swap(_random[i*batch_l+j], temp_l[j*size_l/batch_l+i]);
			}
		}
    }

    static void TearDownTestSuite()
	{
		for(Position *pos_l : _random)
		{
			delete pos_l;
		}

    }

	static std::vector<Position> _continuous;
	static std::vector<Position*> _random;
};

std::vector<Position> CacheTest::_continuous;
std::vector<Position*> CacheTest::_random;

TEST_F(CacheTest, continuous)
{
	auto start = std::chrono::high_resolution_clock::now();
	for(Position & pos_l : _continuous)
	{
		pos_l.x += 12;
		pos_l.y += 15;
	}
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        elapsed).count();
	std::cout<<"time "<<microseconds<<std::endl;
}

TEST_F(CacheTest, random)
{
	auto start = std::chrono::high_resolution_clock::now();
	for(Position * pos_l : _random)
	{
		pos_l->x += 12;
		pos_l->y += 15;
	}
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        elapsed).count();
	std::cout<<"time "<<microseconds<<std::endl;
}