#include "message.h"

Message::Message(QObject *parent)
	: QObject{parent}
{
	data_.push_back("Hello!");
	data_.push_back("Yesterday");
	data_.push_back("All my troubles seemed so far away");
	data_.push_back("Now it looks as though they’re here to stay");
	data_.push_back("Oh, I believe in yesterday.");
	data_.push_back("Suddenly");
	data_.push_back("I’m not half the man I used to be");
	data_.push_back("There’s a shadow hanging over me.");
	data_.push_back("Oh, yesterday came suddenly");
	data_.push_back("Why she");
	data_.push_back("Had to go, I don’t know, she wouldn’t say");
	data_.push_back("I said");
	data_.push_back("Something wrong, now I long for yesterday");
	data_.push_back("Yesterday");
	data_.push_back("Love was such an easy game to play");
	data_.push_back("Now I need a place to hide away");
	data_.push_back("Oh, I believe in yesterday");
}

QString Message::generateText()
{
	int index = rand() % data_.size();
	return data_[index];
}
