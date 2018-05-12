# define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<string>
#include<cmath>
#include<sstream>
#include<iomanip>
#include<cstring>

using namespace std;

const string WEAPON_NAME[3] = { "sword","bomb","arrow" };
const int WEAPON_NUMBER = 3;
const int SOLDIER_NUMBER = 5;

int soldier_life[5];
int soldier_attack[5];
int init_number[5];
const int RED_ORDER[5] = { 2,3,4,1,0 };
const int BLUE_ORDER[5] = { 3,0,1,2,4 };
const string SOLDIER_NAME[5] = { "dragon","ninja","iceman","lion","wolf" };
int g_hour = 0, g_time = 0;

class City;
class Action;
class SoldierBase;

vector<SoldierBase*>red_win_list;
vector<SoldierBase*>blue_win_list;
vector<City*>city_list;



class Weapon
{
public:
	int weapon_name_, weapon_attack_, weapon_life_;
	Weapon(int WEAPON_NAME, int weapon_attack, int weapon_life) :
		weapon_name_(WEAPON_NAME), weapon_attack_(weapon_attack), weapon_life_(weapon_life) {}

	int WeaponUse()
	{
		int weapon_attack = weapon_attack_;
		if (weapon_name_ == 2)
			weapon_life_ -= 1;
		else if (weapon_name_ == 0)
		{
			weapon_attack_ = (int)(weapon_attack_*0.8);
			if (weapon_attack_ == 0)
				weapon_life_ = 0;
		}
		return weapon_attack;
	}

	string WeaponCheck()
	{
		if (weapon_name_ == 0)
			return "sword(" + to_string(weapon_attack_) + ")";
		else if (weapon_name_ == 2)
			return "arrow(" + to_string(weapon_life_) + ")";
		else
			return "bomb";
	}

	int SwordUse()
	{
		if (weapon_name_ == 0)
			return weapon_attack_;
		else
			return 0;
	}

};

class SoldierBase
{
public:
	int soldier_index_, soldier_life_value_, soldier_attack_value_;
	string soldier_name_;

	vector<Weapon>weapon_list_;

public:
	int city_index_, win_tag_, is_alive_, stop_mark_, is_run_;
	int weapon_name_list_[WEAPON_NUMBER];

	SoldierBase(string soldier_name, int soldier_index, int soldier_life_value, int soldier_attack_value) :
		soldier_name_(soldier_name), soldier_index_(soldier_index), soldier_life_value_(soldier_life_value),
		soldier_attack_value_(soldier_attack_value)
	{
		is_alive_ = 1;
		is_run_ = 0;
		win_tag_ = 0;
		stop_mark_ = 0;
		memset(weapon_name_list_, 0, sizeof(weapon_name_list_));
	}

	vector<Weapon> GetWeaponList()
	{
		return weapon_list_;
	}

	int GetLifeValue()
	{
		return soldier_life_value_;
	}

	virtual void SoldierYell()
	{
		return;
	}

	void SetWinTag()
	{
		if (win_tag_ == 0)
			win_tag_ = 1;
		else
			win_tag_ = 0;
	}

	int CheckAlive()
	{
		return is_alive_;
	}

	virtual void SoldierAttack(SoldierBase* enemy)
	{
		printf("%03d:40 ", g_hour);
		int attack_value = soldier_attack_value_;
		if (weapon_name_list_[0] != 0)
		{
			for (auto weapon = weapon_list_.begin(); weapon != weapon_list_.end(); weapon++)
				if (weapon->weapon_name_ == 0)
					attack_value += weapon->WeaponUse();
		}
		string action = soldier_name_ + " " + to_string(soldier_index_) + " attacked " + enemy->GetInformation() + " in city " + to_string(city_index_) + " with " + to_string(soldier_life_value_) +
			" elements and force " + to_string(soldier_attack_value_);
		cout << action << endl;
		enemy->SoldierBeingAttacked(0, this, attack_value);
		return;
	}

	void SoldierFightBack(SoldierBase* enemy)
	{
		printf("%03d:40 ", g_hour);
		int attack_value = (int)soldier_attack_value_*0.5;
		string action = soldier_name_ + " " + to_string(soldier_index_) + " fought back against " + enemy->GetInformation() +
			" in city " + to_string(city_index_);
		cout << action << endl;
		if (weapon_name_list_[0] != 0)
			for (auto weapon = weapon_list_.begin(); weapon != weapon_list_.end(); ++weapon)
			{
				if (weapon->weapon_name_ == 0)
					attack_value += weapon->WeaponUse();
			}
		enemy->SoldierBeingAttacked(1, this, attack_value);
	}

	virtual void SoldierKilled(SoldierBase* enemy, int attack_mark)
	{
		is_alive_ = 0;
		if (attack_mark == 0)
			enemy->AfterWin(this);
	}

	void SoldierBeingAttacked(int attack_kind, SoldierBase* enemy, int enemy_attack_value)
	{
		if (soldier_life_value_ <= enemy_attack_value)
		{
			if (attack_kind == 2)
			{
				soldier_life_value_ = 0;
				printf("%03d:35 ", g_hour);
				cout << enemy->GetInformation() << " shot and killed " << GetInformation() << endl;
				SoldierKilled(enemy, 1);
			}
			else
			{
				printf("%03d:40 ", g_hour);
				string action = soldier_name_ + " " + to_string(soldier_index_) + " was killed in city " + to_string(city_index_);
				cout << action << endl;
				SoldierKilled(enemy, 0);
			}
		}
		else
		{
			soldier_life_value_ -= enemy_attack_value;
			if (attack_kind != 2)
			{
				if (soldier_name_ != "red ninja" && soldier_name_ != "blue ninja"&&attack_kind == 0)
					SoldierFightBack(enemy);
				enemy->NotWin();
			}
			else
			{
				printf("%03d:35 ", g_hour);
				cout << enemy->GetInformation() << " shot" << endl;
			}
		}
	}

	int UseBomb(SoldierBase* enemy, int attack_mark)
	{
		int attack_value = GetAttackValue();

		if ((attack_mark == 0 && soldier_life_value_ <= enemy->GetFightBackValue() && enemy->soldier_life_value_>attack_value) || (attack_mark == 1 && soldier_life_value_ <= enemy->GetAttackValue()))
		{
			printf("%03d:38 ", g_hour);
			string action = GetInformation() + " used a bomb and killed " + enemy->GetInformation();
			cout << action << endl;
			SoldierKilled(enemy, 1);
			enemy->SoldierKilled(this, 1);
			return 1;
		}
		return 0;
	}

	int GetAttackValue()
	{
		if (weapon_name_list_[0] != 0)
		{
			for (auto weapon = weapon_list_.begin(); weapon != weapon_list_.end(); ++weapon)
			{
				if (weapon->weapon_name_ == 0)
					return(soldier_attack_value_ + weapon->SwordUse());
			}
		}
		else
			return soldier_attack_value_;
	}

	int GetFightBackValue()
	{
		if (soldier_name_ == "red ninja" || soldier_name_ == "blue ninja")
			return 0;
		return (int)soldier_attack_value_ * 0.5;
	}

	void UseArrow(SoldierBase* enemy)
	{
		if (weapon_name_list_[2] != 0)
		{
			for (auto weapon = weapon_list_.begin(); weapon != weapon_list_.end(); ++weapon)
			{
				if (weapon->weapon_name_ == 2)
					enemy->SoldierBeingAttacked(2, this, weapon->WeaponUse());
			}
		}
	}

	void GetElements(int elements_number, int attack_number)
	{
		if (attack_number == 0)
			printf("%03d:30 ", g_hour);
		else
			printf("%03d:40 ", g_hour);

		string action = soldier_name_ + " " + to_string(soldier_index_) + " earned " + to_string(elements_number) +
			" elements for his headquarter";
		cout << action << endl;
	}

	void GetWeapon(int WEAPON_NAME, int weapon_attack, int weapon_life)
	{
		if (weapon_name_list_[WEAPON_NAME] == 0)
		{
			Weapon weapon(WEAPON_NAME, weapon_attack, weapon_life);
			weapon_list_.push_back(weapon);
			weapon_name_list_[WEAPON_NAME] = 1;
		}
	}

	void CheckWeapon()
	{
		int comma_mark = 0, weapon_mark = 0;
		printf("%03d:55 ", g_hour);
		string action = soldier_name_ + " " + to_string(soldier_index_) + " has ";
		for (int i = WEAPON_NUMBER - 1; i >= 0; i--)
		{
			if (weapon_name_list_[i] != 0)
			{
				for (auto weapon = weapon_list_.begin(); weapon != weapon_list_.end();)
				{
					if ((*weapon).weapon_name_ != i)
					{
						weapon++;
						continue;
					}
					if ((*weapon).weapon_life_ != 0)
					{
						weapon_mark = 1;
						if (comma_mark)
							action += ",";
						action = action + (*weapon).WeaponCheck();
						comma_mark = 1;
						weapon++;
					}
					else
					{
						weapon_name_list_[i] = 0;
						weapon = weapon_list_.erase(weapon);
					}
				}
			}
		}
		if (weapon_mark == 0)
			action += "no weapon";
		cout << action << endl;
	}

	string GetInformation() const
	{
		return soldier_name_ + " " + to_string(soldier_index_);
	}

	void AttackValueUp(int number)
	{
		soldier_attack_value_ += 20;
	}

	void LifeValueDown(int number)
	{
		soldier_life_value_ -= number;
		if (soldier_life_value_ <= 0)
			soldier_life_value_ = 1;
	}

	virtual void GoForward()
	{
		if (soldier_name_[0] == 'r')
			city_index_ += 1;
		else
			city_index_ -= 1;
	}

	void PrintForward()
	{
		printf("%03d:10 ", g_hour);
		if (city_index_ != 0 && city_index_ != (init_number[1] + 1))
			cout << soldier_name_ + " " + to_string(soldier_index_) + " marched to city " + to_string(city_index_) + " with " +
			to_string(soldier_life_value_) + " elements and force " + to_string(soldier_attack_value_) << endl;
		else if (city_index_ == 0)
		{
			cout << soldier_name_ + " " + to_string(soldier_index_) + " reached red headquarter with " +
				to_string(soldier_life_value_) + " elements and force " + to_string(soldier_attack_value_) << endl;
			stop_mark_ = 1;
		}
		else
		{
			cout << soldier_name_ + " " + to_string(soldier_index_) + " reached blue headquarter with " +
				to_string(soldier_life_value_) + " elements and force " + to_string(soldier_attack_value_) << endl;
			stop_mark_ = 1;
		}
	}

	virtual void AfterWin(SoldierBase* enemy)
	{
		if (soldier_name_[0] == 'r')
			red_win_list.push_back(this);
		else
			blue_win_list.push_back(this);
		SetWinTag();
	}

	virtual void NotWin() {};

	virtual int SoldierRun()
	{
		return 0;
	}
};

class Dragon :SoldierBase
{
private:
	double morale_;
public:
	Dragon(string soldier_name, int soldier_index, int soldier_life_value, int soldier_attack_value, double loyalty) :
		SoldierBase(soldier_name, soldier_index, soldier_life_value, soldier_attack_value), morale_(loyalty)
	{
		int weapon_name = (soldier_index) % 3;
		int weapon_attack = 0, weapon_life = 1;

		if (weapon_name == 0)
		{
			weapon_attack = (int)soldier_attack_value*0.2;
			weapon_life = weapon_attack;
		}
		else if (weapon_name == 2)
		{
			weapon_attack = init_number[2];
			weapon_life = 3;
		}
		SoldierBase::GetWeapon(weapon_name, weapon_attack, weapon_life);
	}

	virtual void AfterWin(SoldierBase* enemy)
	{
		if (SoldierBase::GetInformation()[0] == 'r')
			red_win_list.push_back(this);
		else
			blue_win_list.push_back(this);

		morale_ += 0.2;
		SoldierBase::SetWinTag();
	}

	virtual void NotWin()
	{
		morale_ -= 0.2;
	}

	virtual void SoldierAttack(SoldierBase* enemy)
	{
		printf("%03d:40 ", g_hour);
		int attack_value = soldier_attack_value_;
		if (weapon_name_list_[0] != 0)
			attack_value += weapon_list_[0].WeaponUse();
		string action = SoldierBase::GetInformation() + " attacked " + enemy->GetInformation() + " in city " + to_string(city_index_) + " with " + to_string(soldier_life_value_) +
			" elements and force " + to_string(soldier_attack_value_);
		cout << action << endl;
		enemy->SoldierBeingAttacked(0, this, attack_value);
		SoldierYell();
		return;
	}
	virtual void SoldierYell()
	{
		if (SoldierBase::is_alive_ == 1)
			if (morale_ > 0.8)
			{
				printf("%03d:40 ", g_hour);
				cout << SoldierBase::GetInformation() << " yelled in city " << SoldierBase::city_index_ << endl;
			}
	}
};

class Ninja :SoldierBase
{
public:
	Ninja(string soldier_name, int soldier_index, int soldier_life_value, int soldier_attack_value) :
		SoldierBase(soldier_name, soldier_index, soldier_life_value, soldier_attack_value)
	{
		int weapon_name = (soldier_index) % 3;
		int weapon_attack = 0, weapon_life = 1;

		if (weapon_name == 0)
		{
			weapon_attack = (int)soldier_attack_value*0.2;
			weapon_life = weapon_attack;
		}
		else if (weapon_name == 2)
		{
			weapon_attack = init_number[2];
			weapon_life = 3;
		}
		SoldierBase::GetWeapon(weapon_name, weapon_attack, weapon_life);

		weapon_name = (soldier_index + 1) % 3;
		weapon_attack = 0, weapon_life = 1;
		if (weapon_name == 0)
		{
			weapon_attack = (int)soldier_attack_value*0.2;
			weapon_life = weapon_attack;
		}
		else if (weapon_name == 2)
		{
			weapon_attack = init_number[2];
			weapon_life = 3;
		}
		SoldierBase::GetWeapon(weapon_name, weapon_attack, weapon_life);
	}
};

class Iceman :SoldierBase
{
private:
	int step_number_;
public:
	Iceman(string soldier_name, int soldier_index, int soldier_life_value, int soldier_attack_value) :
		SoldierBase(soldier_name, soldier_index, soldier_life_value, soldier_attack_value), step_number_(0)
	{
		int weapon_name = (soldier_index) % 3;
		int weapon_attack = 0, weapon_life = 1;

		if (weapon_name == 0)
		{
			weapon_attack = (int)soldier_attack_value*0.2;
			weapon_life = weapon_attack;
		}
		else if (weapon_name == 2)
		{
			weapon_attack = init_number[2];
			weapon_life = 3;
		}
		SoldierBase::GetWeapon(weapon_name, weapon_attack, weapon_life);
	}

	virtual void GoForward()
	{
		step_number_ += 1;
		if (step_number_ == 2)
		{
			SoldierBase::AttackValueUp(20);
			SoldierBase::LifeValueDown(9);
			step_number_ = 0;
		}
		if (SoldierBase::GetInformation()[0] == 'r')
			city_index_ += 1;
		else
			city_index_ -= 1;
	}
};

class Lion :SoldierBase
{
private:
	double loyality_;
public:
	Lion(string soldier_name, int soldier_index, int soldier_life_value, int soldier_attack_value, double loyality) :
		SoldierBase(soldier_name, soldier_index, soldier_life_value, soldier_attack_value), loyality_(loyality) {}
	void NotWin()
	{
		loyality_ -= init_number[3];
	}

	virtual void SoldierKilled(SoldierBase* enemy, int attack_mark)
	{
		is_alive_ = 0;
		if (attack_mark == 0)
		{
			enemy->SetWinTag();
			enemy->LifeValueDown(-SoldierBase::GetLifeValue());
			enemy->AfterWin(this);
		}
	}

	virtual int SoldierRun()
	{
		if (loyality_ <= 0)
		{
			printf("%03d:05 ", g_hour);
			cout << SoldierBase::GetInformation() << " ran away " << endl;
			SoldierBase::is_run_ = 1;
			return 1;
		}
		return 0;
	}
};

class Wolf :SoldierBase
{
public:
	Wolf(string soldier_name, int soldier_index, int soldier_life_value, int soldier_attack_value) :
		SoldierBase(soldier_name, soldier_index, soldier_life_value, soldier_attack_value) {}

	virtual void AfterWin(SoldierBase* enemy)
	{
		vector<Weapon> weapon_list = enemy->GetWeaponList();
		for (auto weapon = weapon_list.begin(); weapon != weapon_list.end(); ++weapon)
			SoldierBase::GetWeapon((*weapon).weapon_name_, (*weapon).weapon_attack_, (*weapon).weapon_life_);
		SoldierBase::SetWinTag();
		if (soldier_name_[0] == 'r')
			red_win_list.push_back(this);
		else
			blue_win_list.push_back(this);
	}
};

class HeadQuarter
{
	friend Action;
private:
	string hq_name_, hq_colour_;
	int soldier_order_[SOLDIER_NUMBER];
	int life_elements_, soldier_index_, enemy_number_;
	vector<SoldierBase*>soldier_list_;
public:
	void set_value(string hq_colour, const int soldier_order[5], int life_elements)
	{
		hq_colour_ = hq_colour;
		hq_name_ = hq_colour_ + " headquarter";
		life_elements_ = life_elements;
		for (int i = 0; i < SOLDIER_NUMBER; ++i)
			soldier_order_[i] = soldier_order[i];
		soldier_index_ = 1;
		enemy_number_ = 0;
	}

	int CreatSoldier()
	{
		int soldier_switch = soldier_order_[(soldier_index_ - 1) % 5];
		int life_value = soldier_life[soldier_switch];
		int attack_value = soldier_attack[soldier_switch];
		if (life_elements_ <life_value)
			return 0;
		printf("%03d:00 ", g_hour);
		string soldier_name = hq_colour_ + " " + SOLDIER_NAME[soldier_switch];
		life_elements_ -= life_value;
		cout << soldier_name << " " << soldier_index_ << " born" << endl;
		double morale = 0;
		stringstream temp;
		switch (soldier_switch)
		{
		case 0:
			morale = (double)life_elements_ / life_value;
			temp << fixed << setprecision(2) << morale;
			temp >> morale;
			soldier_list_.push_back((SoldierBase*) new Dragon(soldier_name, soldier_index_, life_value, attack_value, morale));
			cout << "Its morale is " << fixed << setprecision(2) << morale << endl;
			break;
		case 1:
			soldier_list_.push_back((SoldierBase*) new Ninja(soldier_name, soldier_index_, life_value, attack_value));
			break;
		case 2:
			soldier_list_.push_back((SoldierBase*) new Iceman(soldier_name, soldier_index_, life_value, attack_value));
			break;
		case 3:
			soldier_list_.push_back((SoldierBase*) new Lion(soldier_name, soldier_index_, life_value, attack_value, (int)life_elements_));
			cout << "Its loyalty is " << (int)life_elements_ << endl;
			break;
		case 4:
			soldier_list_.push_back((SoldierBase*) new Wolf(soldier_name, soldier_index_, life_value, attack_value));
			break;
		default:
			break;
		}
		soldier_index_++;
		if (soldier_name[0] == 'r')
			soldier_list_.back()->city_index_ = 0;
		else
			soldier_list_.back()->city_index_ = init_number[1] + 1;
		return 1;
	}

	void GiveLife(SoldierBase* soldier)
	{
		if (life_elements_ > 8)
		{
			soldier->LifeValueDown(-8);
			life_elements_ -= 8;
		}
	}

	void GetElements(int life_elements_change)
	{
		life_elements_ += life_elements_change;
	}

	void PrintElementsNumber()
	{
		printf("%03d:50 ", g_hour);
		cout << life_elements_ << " elements in " << hq_name_ << endl;
	}

	void BeingTaken()
	{
		printf("%03d:10 ", g_hour);
		cout << hq_name_ << " was taken" << endl;
	}

};

class City
{
	friend Action;
private:
	int city_index_, store_elements_, flag_number_, flag_mark_;
	SoldierBase* red_soldier_;
	SoldierBase* blue_soldier_;
public:
	int attack_mark_;
	City(int city_index) :city_index_(city_index)
	{
		flag_mark_ = 0;
		attack_mark_ = (city_index % 2 == 0) ? 1 : -1;
		store_elements_ = 0;
		flag_number_ = 0;
		red_soldier_ = NULL;
		blue_soldier_ = NULL;
	}

	void AddSoldier(SoldierBase* soldier)
	{
		if (soldier->GetInformation()[0] == 'r')
			red_soldier_ = soldier;
		else
			blue_soldier_ = soldier;
	}

	void SoldierDelete(SoldierBase* soldier)
	{
		if (soldier->GetInformation()[0] == 'r')
			red_soldier_ = NULL;
		else
			blue_soldier_ = NULL;
	}

	void SoldierAttack()
	{
		if (red_soldier_ != NULL && blue_soldier_ != NULL)
		{
			if (flag_number_ == -2 && (city_index_ % 2) == 1)
				red_soldier_->SoldierAttack(blue_soldier_);
			else
				blue_soldier_->SoldierAttack(red_soldier_);
		}

		if (red_soldier_->CheckAlive() == 0)
			red_soldier_ = NULL;
		if (blue_soldier_->CheckAlive() == 0)
			blue_soldier_ = NULL;
	}

	void RaiseFlag()
	{
		if (flag_number_ == -2 && flag_mark_ != -1)
		{
			printf("%03d:40 ", g_hour);
			cout << "red flag raised in city " << city_index_ << endl;
			attack_mark_ = -1;
			flag_mark_ = -1;
		}
		else if (flag_number_ == 2 && flag_mark_ != 1)
		{
			printf("%03d:40 ", g_hour);
			cout << "blue flag raised in city " << city_index_ << endl;
			attack_mark_ = 1;
			flag_mark_ = 1;
		}
	}

};

class Action
{
	HeadQuarter red, blue;
public:
	void InitGame()
	{
		city_list.clear();
		memset(soldier_life, 0, 5 * sizeof(int));
		memset(init_number, 0, 5 * sizeof(int));
		memset(soldier_attack, 0, 5 * sizeof(int));
		g_hour = 0;
		g_time = 0;
		for (int j = 0; j < 5; ++j)
			cin >> init_number[j];
		for (int j = 0; j < 5; ++j)
			cin >> soldier_life[j];
		for (int j = 0; j < 5; ++j)
			cin >> soldier_attack[j];
		for (int j = 0; j < init_number[1] + 2; ++j)
			city_list.push_back((City*)new City(j));
		string name = "red";
		red.set_value(name, RED_ORDER, init_number[0]);
		name = "blue";
		blue.set_value(name, BLUE_ORDER, init_number[0]);
	}

	void CreateSoldier()
	{
		if (red.CreatSoldier())
			city_list[0]->AddSoldier(red.soldier_list_.back());
		if (blue.CreatSoldier())
			city_list[init_number[1] + 1]->AddSoldier(blue.soldier_list_.back());
		g_time += 5;
	}

	void CheckRun()
	{
		if ((*city_list.begin())->red_soldier_ != NULL)
		{
			if ((*city_list.begin())->red_soldier_->SoldierRun() == 1)
				(*city_list.begin())->red_soldier_ = NULL;
		}
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			if ((*city)->red_soldier_ != NULL)
			{
				if ((*city)->red_soldier_->SoldierRun() == 1)
					(*city)->red_soldier_ = NULL;
			}
			if ((*city)->blue_soldier_ != NULL)
			{
				if ((*city)->blue_soldier_->SoldierRun())
					(*city)->blue_soldier_ = NULL;
			}
		}
		if ((*city_list.rbegin())->blue_soldier_ != NULL)
		{
			if ((*city_list.rbegin())->blue_soldier_->SoldierRun() == 1)
				(*city_list.rbegin())->blue_soldier_ = NULL;
		}
		g_time += 5;
	}

	int GoForward()
	{
		int red_taken_mark = 0, blue_taken_mark = 0;

		if ((*city_list.begin())->red_soldier_ != NULL)
		{
			(*city_list.begin())->red_soldier_->GoForward();
			(*city_list.begin())->red_soldier_ = NULL;
		}
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			if ((*city)->red_soldier_ != NULL)
			{
				(*city)->red_soldier_->GoForward();
				if (((*city)->red_soldier_->city_index_ == init_number[1] + 1) && city_list[init_number[1] + 1]->red_soldier_ != NULL)
					blue_taken_mark = 1;
			}
			if ((*city)->blue_soldier_ != NULL)
			{
				(*city)->blue_soldier_->GoForward();
				if (((*city)->blue_soldier_->city_index_ == 0) && city_list[0]->blue_soldier_ != NULL)
					red_taken_mark = 1;
			}
			(*city)->red_soldier_ = NULL;
			(*city)->blue_soldier_ = NULL;
		}
		if (city_list[init_number[1] + 1]->blue_soldier_ != NULL)
		{
			city_list[init_number[1] + 1]->blue_soldier_->GoForward();
			city_list[init_number[1] + 1]->blue_soldier_ = NULL;
		}

		for (auto soldier = red.soldier_list_.begin(); soldier != red.soldier_list_.end(); soldier++)
		{
			if ((*soldier)->CheckAlive() == 1 && (*soldier)->is_run_ == 0)
				city_list[(*soldier)->city_index_]->AddSoldier(*soldier);
		}

		for (auto soldier = blue.soldier_list_.begin(); soldier != blue.soldier_list_.end(); soldier++)
		{
			if ((*soldier)->CheckAlive() && (*soldier)->is_run_ == 0)
				city_list[(*soldier)->city_index_]->AddSoldier(*soldier);
		}

		if ((city_list[0]->blue_soldier_) != NULL && (city_list[0]->blue_soldier_->stop_mark_) != 1)
			city_list[0]->blue_soldier_->PrintForward();
		if (red_taken_mark == 1)
			red.BeingTaken();
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			if (((*city)->red_soldier_) != NULL)
				(*city)->red_soldier_->PrintForward();
			if (((*city)->blue_soldier_) != NULL)
				(*city)->blue_soldier_->PrintForward();
		}
		if ((city_list[init_number[1] + 1]->red_soldier_) != NULL && (city_list[init_number[1] + 1]->red_soldier_->stop_mark_) != 1)
			city_list[init_number[1] + 1]->red_soldier_->PrintForward();
		if (blue_taken_mark == 1)
			blue.BeingTaken();
		g_time += 10;
		return (red_taken_mark || blue_taken_mark);
	}

	void ArrowAttack()
	{
		if (city_list.size() <= 3)
			return;
		for (auto city = city_list.begin(); city != city_list.begin() + 2; ++city)
		{
			if ((*city)->red_soldier_ != NULL && (*(city + 1))->blue_soldier_ != NULL)
				(*city)->red_soldier_->UseArrow((*(city + 1))->blue_soldier_);
		}
		for (auto city = city_list.begin() + 2; city != city_list.end() - 2; ++city)
		{
			if ((*city)->red_soldier_ != NULL && (*(city + 1))->blue_soldier_ != NULL)
				(*city)->red_soldier_->UseArrow((*(city + 1))->blue_soldier_);
			if ((*city)->blue_soldier_ != NULL && (*(city - 1))->red_soldier_ != NULL)
				(*city)->blue_soldier_->UseArrow((*(city - 1))->red_soldier_);
		}
		for (auto city = city_list.rbegin(); city != city_list.rbegin() + 2; ++city)
		{
			if ((*city)->blue_soldier_ != NULL && (*(city + 1))->red_soldier_ != NULL)
				(*city)->blue_soldier_->UseArrow((*(city + 1))->red_soldier_);
		}
		g_time += 3;
	}

	void BombAttack()
	{
		g_time += 2;
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			if ((*city)->red_soldier_ != NULL && (*(city))->blue_soldier_ != NULL)
				if ((*city)->red_soldier_->is_alive_ == 1 && (*(city))->blue_soldier_->is_alive_ == 1)
				{
					{
						if ((*city)->red_soldier_->weapon_name_list_[1] == 1 && (*city)->red_soldier_->is_alive_ == 1)
						{
							int attack_temp = ((*city)->attack_mark_ == -1) ? 0 : 1;
							if ((*city)->red_soldier_->UseBomb((*city)->blue_soldier_, attack_temp))
							{
								(*city)->red_soldier_ = NULL;
								(*city)->blue_soldier_ = NULL;
								continue;
							}
						}
						if ((*city)->blue_soldier_->weapon_name_list_[1] == 1 && (*city)->blue_soldier_->is_alive_ == 1)
						{
							int attack_temp = ((*city)->attack_mark_ == 1) ? 0 : 1;
							if ((*city)->blue_soldier_->UseBomb((*city)->red_soldier_, attack_temp))
							{
								(*city)->red_soldier_ = NULL;
								(*city)->blue_soldier_ = NULL;
							}
						}
					}
				}
		}
	}

	void SoldierAttack()
	{
		int red_elements = 0, blue_elements = 0;
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			if ((*city)->red_soldier_ != NULL && (*(city))->blue_soldier_ != NULL)
			{
				if ((*city)->red_soldier_->is_alive_ == 0 || (*city)->blue_soldier_->is_alive_ == 0)
				{
					if ((*city)->red_soldier_->is_alive_ == 0 && (*city)->blue_soldier_->is_alive_ == 1)
					{
						(*city)->red_soldier_->SoldierKilled((*city)->blue_soldier_, 0);
						if ((*city)->blue_soldier_->GetInformation().find("dragon") > 0 && (*city)->attack_mark_ == 1)
							(*city)->blue_soldier_->SoldierYell();
					}
					else if ((*city)->red_soldier_->is_alive_ == 1 && (*city)->blue_soldier_->is_alive_ == 0)
					{
						(*city)->blue_soldier_->SoldierKilled((*city)->red_soldier_, 0);
						if ((*city)->red_soldier_->GetInformation().find("dragon") > 0 && (*city)->attack_mark_ == -1)
							(*city)->red_soldier_->SoldierYell();
					}
					else
					{
						(*city)->red_soldier_ = NULL;
						(*city)->blue_soldier_ = NULL;
					}
				}
				else
				{
					if ((*city)->attack_mark_ == -1)
						(*city)->red_soldier_->SoldierAttack((*(city))->blue_soldier_);
					else if ((*city)->attack_mark_ == 1)
						(*city)->blue_soldier_->SoldierAttack((*(city))->red_soldier_);
				}
			}
			if ((*city)->red_soldier_ != NULL && (*(city))->blue_soldier_ != NULL)
			{
				if ((*city)->red_soldier_->CheckAlive() == 0)
				{
					(*city)->red_soldier_ = NULL;
					(*city)->blue_soldier_->GetElements((*city)->store_elements_, 1);
					if ((*city)->flag_number_ < 0)
						(*city)->flag_number_ = 0;
					(*city)->flag_number_ += 1;
					blue_elements += (*city)->store_elements_;
					(*city)->store_elements_ = 0;
				}
				else if ((*city)->blue_soldier_->CheckAlive() == 0)
				{
					(*city)->blue_soldier_ = NULL;
					(*city)->red_soldier_->GetElements((*city)->store_elements_, 1);
					if ((*city)->flag_number_ > 0)
						(*city)->flag_number_ = 0;
					(*city)->flag_number_ -= 1;
					red_elements += (*city)->store_elements_;
					(*city)->store_elements_ = 0;
				}
				else
					(*city)->flag_number_ = 0;
				if ((*city)->flag_number_ == 2 || (*city)->flag_number_ == -2)
				{
					(*city)->RaiseFlag();
					(*city)->flag_number_ = 0;
				}
			}
			if ((*city)->red_soldier_ != NULL && (*(city))->red_soldier_->is_alive_ == 0)
				(*city)->red_soldier_ = NULL;
			if ((*city)->blue_soldier_ != NULL && (*city)->blue_soldier_->is_alive_ == 0)
				(*city)->blue_soldier_ = NULL;
		}

		for (auto soldier = red_win_list.rbegin(); soldier != red_win_list.rend(); ++soldier)
			red.GiveLife(*soldier);
		for (auto soldier = blue_win_list.begin(); soldier != blue_win_list.end(); ++soldier)
			blue.GiveLife(*soldier);
		red.GetElements(red_elements);
		blue.GetElements(blue_elements);
		g_time += 10;
	}

	void PrintElements()
	{
		red.PrintElementsNumber();
		blue.PrintElementsNumber();
		g_time += 5;
	}

	void GetElements()
	{
		int red_elements = 0, blue_elements = 0;
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			if ((*city)->blue_soldier_ == NULL && (*city)->red_soldier_ != NULL)
			{
				(*city)->red_soldier_->GetElements((*city)->store_elements_, 0);
				red_elements += (*city)->store_elements_;
				(*city)->store_elements_ = 0;
			}
			if ((*city)->blue_soldier_ != NULL && (*city)->red_soldier_ == NULL)
			{
				(*city)->blue_soldier_->GetElements((*city)->store_elements_, 0);
				blue_elements += (*city)->store_elements_;
				(*city)->store_elements_ = 0;
			}
		}
		red.GetElements(red_elements);
		blue.GetElements(blue_elements);
		g_time += 5;
	}

	void PrintWeapons()
	{
		for (auto city = city_list.begin(); city != city_list.end(); ++city)
		{
			if ((*city)->red_soldier_ != NULL)
				(*city)->red_soldier_->CheckWeapon();
		}
		for (auto city = city_list.begin(); city != city_list.end(); ++city)
		{
			if ((*city)->blue_soldier_ != NULL)
				(*city)->blue_soldier_->CheckWeapon();
		}
		g_time += 5;
	}

	void GenerateElements()
	{
		for (auto city = city_list.begin() + 1; city != city_list.end() - 1; ++city)
		{
			(*city)->store_elements_ += 10;
		}
		g_time += 10;
	}
};

int main()
{
	int t = 0;
	cin >> t;
	for (int i = 1; i <= t; ++i)
	{
		Action action;

		action.InitGame();
		cout << "Case " << i << ":" << endl;
		while (1)
		{
			red_win_list.clear();
			blue_win_list.clear();
			if (g_time >init_number[4])
				break;
			action.CreateSoldier();
			if (g_time >init_number[4])
				break;
			action.CheckRun();
			if (g_time >init_number[4])
				break;
			if (action.GoForward())
				break;
			if (g_time > init_number[4])
				break;
			action.GenerateElements();
			if (g_time > init_number[4])
				break;
			action.GetElements();
			if (g_time > init_number[4])
				break;
			action.ArrowAttack();
			if (g_time > init_number[4])
				break;
			action.BombAttack();
			if (g_time > init_number[4])
				break;
			action.SoldierAttack();
			if (g_time > init_number[4])
				break;
			action.PrintElements();
			if (g_time > init_number[4])
				break;
			action.PrintWeapons();
			g_hour++;
		}
	}
	return 0;
}