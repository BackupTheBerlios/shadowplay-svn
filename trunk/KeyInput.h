#ifndef KEYINPUT_H
#define KEYINPUT_H

class KeyInput
{
  public:
	KeyInput(void);
	~KeyInput(void);

	int CheckKeys(void);
	bool Running(void);

  private:
	bool running;

  protected:
};

#endif
