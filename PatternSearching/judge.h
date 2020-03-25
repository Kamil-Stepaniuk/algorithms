class ISolution {
public:
	virtual int count(char text[], char phrase[]) = 0;
};

class Judge
{
public:
	static void run(ISolution *solution);
};

