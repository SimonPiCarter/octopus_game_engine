#include <gtest/gtest.h>

#include <command/Command.hh>
#include <command/CommandQueue.hh>
#include <state/State.hh>
#include <step/Step.hh>

///
/// This test suite aims at checking that EntityMoveStep works properly
/// - Move an entity in a step
/// - Is correctly undone
///

using namespace octopus;

/// @brief test command that just stores an integer
/// in the data to push it on the back of the vec
class TestCommand : public Command
{
public:
	TestCommand(unsigned long val_p, std::vector<unsigned long> &vec_p) : Command(0), _val(val_p), _vec(vec_p) {}

	virtual bool applyCommand(Step &, State const &, CommandData const * data_p) const
	{
		_vec.push_back(static_cast<CommandDataWithData<unsigned long> const *>(data_p)->_data);

		return true;
	}

	/// @brief create data supporting the command actions
	virtual CommandData * newData() const
	{
		return new CommandDataWithData<unsigned long>(_val);
	}

protected:
	unsigned long _val;
	std::vector<unsigned long> &_vec;
};

TEST(commandQueueTest, throw_test)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	EXPECT_FALSE(queue_l.hasCommand());
	EXPECT_THROW(queue_l.getCurrentCommand(), std::exception);
}

TEST(commandQueueTest, simple)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	TestCommand cmd_l(1, vec_l);

	queue_l.queueCommand(&cmd_l);

	EXPECT_TRUE(queue_l.hasCommand());
	CommandQueue::ConstQueueIterator it_l = queue_l.getCurrentCommand();

	while(it_l != queue_l.getEnd())
	{
		if(it_l->_cmd->applyCommand(step_l, state_l, it_l->_data))
		{
			++it_l;
			queue_l.nextCommand();
		}
		else
		{
			break;
		}
	}

	ASSERT_EQ(1u, vec_l.size());
	EXPECT_EQ(1u, vec_l.at(0));
	EXPECT_EQ(queue_l.getEnd(), queue_l.getCurrentCommand());
}

TEST(commandQueueTest, multiple)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	TestCommand cmd_l(1, vec_l);
	TestCommand cmd2_l(2, vec_l);
	TestCommand cmd3_l(3, vec_l);

	queue_l.queueCommand(&cmd_l);
	queue_l.queueCommandLast(&cmd2_l);
	queue_l.queueCommandLast(&cmd3_l);

	EXPECT_TRUE(queue_l.hasCommand());
	CommandQueue::ConstQueueIterator it_l = queue_l.getCurrentCommand();

	while(it_l != queue_l.getEnd())
	{
		if(it_l->_cmd->applyCommand(step_l, state_l, it_l->_data))
		{
			++it_l;
			queue_l.nextCommand();
		}
		else
		{
			break;
		}
	}

	ASSERT_EQ(3u, vec_l.size());
	EXPECT_EQ(1u, vec_l.at(0));
	EXPECT_EQ(2u, vec_l.at(1));
	EXPECT_EQ(3u, vec_l.at(2));
	ASSERT_EQ(queue_l.getEnd(), queue_l.getCurrentCommand());
}

TEST(commandQueueTest, multiple_override)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	TestCommand cmd_l(1, vec_l);
	TestCommand cmd2_l(2, vec_l);
	TestCommand cmd3_l(3, vec_l);

	queue_l.queueCommand(&cmd_l);
	queue_l.queueCommandLast(&cmd2_l);
	queue_l.queueCommand(&cmd3_l);

	EXPECT_TRUE(queue_l.hasCommand());
	CommandQueue::ConstQueueIterator it_l = queue_l.getCurrentCommand();

	while(it_l != queue_l.getEnd())
	{
		if(it_l->_cmd->applyCommand(step_l, state_l, it_l->_data))
		{
			++it_l;
			queue_l.nextCommand();
		}
		else
		{
			break;
		}
	}

	ASSERT_EQ(1u, vec_l.size());
	EXPECT_EQ(3u, vec_l.at(0));
	ASSERT_EQ(queue_l.getEnd(), queue_l.getCurrentCommand());
}

TEST(commandQueueTest, multiple_override_reverted)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	TestCommand cmd_l(1, vec_l);
	TestCommand cmd2_l(2, vec_l);
	TestCommand cmd3_l(3, vec_l);

	queue_l.queueCommandLast(&cmd_l);
	queue_l.queueCommandLast(&cmd2_l);
	queue_l.queueCommand(&cmd3_l);

	queue_l.unqueueCommand(&cmd3_l);

	EXPECT_TRUE(queue_l.hasCommand());
	CommandQueue::ConstQueueIterator it_l = queue_l.getCurrentCommand();

	while(it_l != queue_l.getEnd())
	{
		if(it_l->_cmd->applyCommand(step_l, state_l, it_l->_data))
		{
			++it_l;
			queue_l.nextCommand();
		}
		else
		{
			break;
		}
	}

	ASSERT_EQ(2u, vec_l.size());
	EXPECT_EQ(1u, vec_l.at(0));
	EXPECT_EQ(2u, vec_l.at(1));
	ASSERT_EQ(queue_l.getEnd(), queue_l.getCurrentCommand());
}

TEST(commandQueueTest, multiple_override_reverted_throw)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	TestCommand cmd_l(1, vec_l);
	TestCommand cmd2_l(2, vec_l);
	TestCommand cmd3_l(3, vec_l);

	queue_l.queueCommand(&cmd_l);
	queue_l.queueCommandLast(&cmd2_l);
	queue_l.queueCommand(&cmd3_l);

	EXPECT_THROW(queue_l.unqueueCommand(&cmd2_l), std::exception);
}

TEST(commandQueueTest, multiple_override_reverted_all)
{
	State state_l;
	Step step_l;

	std::vector<unsigned long> vec_l;

	CommandQueue queue_l;

	TestCommand cmd_l(1, vec_l);
	TestCommand cmd2_l(2, vec_l);
	TestCommand cmd3_l(3, vec_l);

	queue_l.queueCommandLast(&cmd_l);
	queue_l.queueCommandLast(&cmd2_l);
	queue_l.queueCommand(&cmd3_l);

	queue_l.unqueueCommand(&cmd3_l);
	queue_l.unqueueCommandLast(&cmd2_l);
	queue_l.unqueueCommandLast(&cmd_l);

	queue_l.queueCommandLast(&cmd_l);
	queue_l.queueCommandLast(&cmd2_l);
	queue_l.queueCommandLast(&cmd3_l);

	EXPECT_TRUE(queue_l.hasCommand());
	CommandQueue::ConstQueueIterator it_l = queue_l.getCurrentCommand();

	while(it_l != queue_l.getEnd())
	{
		if(it_l->_cmd->applyCommand(step_l, state_l, it_l->_data))
		{
			++it_l;
			queue_l.nextCommand();
		}
		else
		{
			break;
		}
	}

	ASSERT_EQ(3u, vec_l.size());
	EXPECT_EQ(1u, vec_l.at(0));
	EXPECT_EQ(2u, vec_l.at(1));
	EXPECT_EQ(3u, vec_l.at(2));
	ASSERT_EQ(queue_l.getEnd(), queue_l.getCurrentCommand());
}
