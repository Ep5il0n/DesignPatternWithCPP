https://github.com/kamranahmedse/design-patterns-for-humans?tab=readme-ov-file#-bridge

// Let's say we are designing a logging system where 
// log messages can be filtered and handled by different loggers,
//  such as ConsoleLogger, FileLogger, and ErrorLogger. 
//  Each logger should decide whether it can handle the 
//  log message or pass it on to the next logger in the chain.

////////////////////////////////
// Chain of responsibility ////
///////////////////////////////
#include<iostream>
#include<algorithm>
#include<cstring>
#include<string>
#include<stack>
#include<map>
#include<unordered_map>
#include<set>

class Logger {
    protected:
        Logger* nextLogger;
    public:
        Logger(): nextLogger(nullptr){}
        void setNextLogger(Logger* next) {
            nextLogger = next;
        }
        void logMessage(int level, std::string msg) {
            if(canHandle(level)) {
                write(msg);
            }
            if(nextLogger!=nullptr) {
                nextLogger->logMessage(level,msg);
            }
        }

        virtual bool canHandle(int level) = 0;
        virtual void write(std::string msg) = 0;
};

class ConsoleLogger: public Logger {
    private:
        int logLevel;
    public:
        ConsoleLogger(int level): logLevel(level){}
        bool canHandle(int level) override {
            return level >= logLevel;
        }
        void write(std::string msg) override {
            std::cout<<"msg"<<std::endl;
        }

};

class FileLogger: public Logger {
    private:
        int logLevel;
    public:
        FileLogger(int level): logLevel(level){}
        bool canHandle(int level) override {
            return level >= logLevel;
        }
        void write(std::string msg) override {
            std::cout<<"msg"<<std::endl;
        }
        
};

class ErrorLogger: public Logger {
    private:
        int logLevel;
    public:
        ErrorLogger(int level): logLevel(level){}
        bool canHandle(int level) override {
            return level >= logLevel;
        }
        void write(std::string msg) override {
            std::cout<<"msg"<<std::endl;
        }
        
};

int main() {
    Logger* conSoleLogger = new ConsoleLogger(1);
    Logger* fileLogger = new FileLogger(2);
    Logger* errorLogger = new ErrorLogger(3);

    errorLogger->setNextLogger(fileLogger);
    fileLogger->setNextLogger(conSoleLogger);

    errorLogger->logMessage(3,"this is a hotel level message");
}



// Imagine we are building a simple text editor that can execute commands like
// "write text," "undo the last command," and "redo a command."
// The Command pattern can encapsulate these actions as objects.

///////////////////////////////
// Command ////////////////////
///////////////////////////////


class Command {
    public:
        virtual void execute() = 0;
        virtual void undo() = 0;
};

class TextEditor {
    private: 
        std::string text;
    public:
        void writeText(std::string newText) {
            text+=newText;
        }
        void removeText(int length) {
            text.erase(text.length()-length, length);
        }

        void showText() {
            std::cout<<text<<std::endl;
        }
};

class WriteCommand: public Command {
    private:
        TextEditor* textEditor;
        std::string text;
    public:
        WriteCommand(TextEditor *TextEditor, std::string Text): textEditor(TextEditor), text(Text) {}

    void execute() override {
        textEditor->writeText(text);
    }

    void undo() override {
        textEditor->removeText(text.length());
    }
};

class CommandManagaer {
    private:
        std::stack<Command*>commandHistory;
        std::stack<Command*>redo;
    public:
        void executeCommand(Command* command) {
            command->execute();
            commandHistory.push(command);

        }

        void undo() {
            Command* lastCommand = commandHistory.top();
            lastCommand->undo();
            commandHistory.pop();
            redo.push(lastCommand);
        }

        void redoCommand() {
            Command* redoCommand = redo.top();
            redoCommand->execute();
            commandHistory.push(redoCommand);
            redo.pop();
        }
};

int main() {
    TextEditor* textEditor;
    CommandManagaer* commandManager;
    Command* command1 = new WriteCommand(textEditor, "hello");
    Command* command2 = new WriteCommand(textEditor, "world");

    commandManager->executeCommand(command1);
    textEditor->showText();
    commandManager->executeCommand(command2);
    textEditor->showText();
    commandManager->undo();
    commandManager->undo();
    commandManager->redoCommand();
}


// Design a vending machine
// Vending Machine must keep track of the inventory
// A person should be able to insert cash into the machine & choose an item
// The Machine should confirm the inserted cash with the price of the selected item
// The machine must display an error in case of insufficient cash or unavailable item
// Finally, if all the above steps succeed then the user gets the selected item

////////////////////////////////
// state  /////////////////////
///////////////////////////////


class VendingMachine;

class State {
    virtual void insertCash(int cash, VendingMachine* machine) = 0;
    virtual void chooseItem(std::string item, VendingMachine* machine) = 0;
    virtual void dispenseItem(VendingMachine* machine) = 0;
};

class NoCashState: public State {
    public:
        void insertCash(int cash, VendingMachine* macine) override;
        void chooseItem(std::string item, VendingMachine* machine) override {
            std::cout<<"please insert cash first";
        }
        void dispenseItem(VendingMachine* machine) override {
            std::cout<<"no cash inserted, cannot dispesne item";
        }

};

class HasCashState: public State {
    public:
        void insertCash(int cash, VendingMachine* macine) override {
            std::cout<<"Cash already inserted, select an item";
        }
        void chooseItem(std::string item, VendingMachine* machine) override;
        void dispenseItem(VendingMachine* machine) override {
            std::cout<<"No item chosen, choose an item";
        }
};

class ItemSelectedState: public State {
     public:
        void insertCash(int cash, VendingMachine* macine) override {
            std::cout<<"Cash already inserted, select an item";
        }
        void chooseItem(std::string item, VendingMachine* machine) override {
            std::cout<<"Item already selected";
        }
        void dispenseItem(VendingMachine* machine) override;
};

class SoldState : public State {
public:
    void insertCash(VendingMachine* machine, int amount) override {
        std::cout << "Please wait, dispensing your item.\n";
    }
    void chooseItem(VendingMachine* machine, const std::string& item) override {
        std::cout << "Please wait, dispensing your item.\n";
    }
    void dispenseItem(VendingMachine* machine) override {
        std::cout << "Dispensing your item...\n";
        // Transition to NoCashState after dispensing
    }
};

void NoCashState::insertCash(int cash, VendingMachine* machine) {
    std::cout<<"Inserted amount: "<<cash;
    machine->setState(machine->getHasCashState());
}

void HasCashState:: chooseItem(std::string item, VendingMachine* machine) {
    if(machine->hasItem(item)) {
        if(machine->getItemPrice(item) <= machine->getCurrentCash()) {
            // reduce item
            // update cash
            machine->setState(machine->getItemSelectedState());
            machine->dispenseItem();
        }
    } else {
        std::cout<<"Item not available";
    }
}

void ItemSelectedState:: dispenseItem(VendingMachine* machine) {
    machine->setState(machine->getNoCashState());
}

void SoldState::dispenseItem(VendingMachine* machine) {
    std::cout << "Dispensing item...\n";
    machine->setState(machine->getNoCashState());
}

class VendingMachine {
    private:
        State* noCashState;
        State* hashCashState;
        State* itemSelectedstate;
        State* soldState;
        State* currentState;
        int currentCash;
        std::unordered_map<std::string,int>itemList;
    public:
        VendingMachine():noCashState(new NoCashState()), hashCashState(new HasCashState()), itemSelectedstate(new ItemSelectedState()), currentState(noCashState), currentCash(0)
        {
            itemList["Soda"] = 20;
            itemList["Coke"] = 5;
        } 

        void setState(State* newState) {
            currentState = newState;
        }

        void insertCash(int cash) {
            currentCash+=cash;
            currentState->insertCash(cash, this);
        }

        void chooseItem(std::string item) {
            currentState->chooseItem(item, this);
        }

        void dispenseItem() {
            currentState->dispenseItem(this);
        }

        bool hasItem(std::string item) {
            return itemList[item] > 0;
        }

        bool getItemPrice(std::string item) {
            return 25;
        }

        int getCurrentCash() {
            return currentCash;
        }

        State* getHasCashState() {
            return hashCashState;
        }

        State* getNoCashState() {
            return noCashState;
        }
        
        State* getSoldState() {
            return soldState;
        }

        State* getItemSelectedState() {
            return itemSelectedstate;
        }

};


int main() {
    VendingMachine* machine = new VendingMachine();
    machine->insertCash(50);
    machine->chooseItem("Soda");
    machine->insertCash(20);
}

// Design Unix File Search API to search file with different 
// arguments as "extension", "name", "size" ... using filter design pattern 

////////////////////////////////
// Criteria   /////////////////
///////////////////////////////

class File {
private:
    std::string name;
    std::string extension;
    int size; // Size in KB

public:
    File(const std::string& name, const std::string& extension, int size)
        : name(name), extension(extension), size(size) {}

    std::string getName() const { return name; }
    std::string getExtension() const { return extension; }
    int getSize() const { return size; }
};

class Filter {
public:
    virtual ~Filter() {}
    virtual std::vector<File> meetCriteria(const std::vector<File>& files) const = 0;
};

class ExtensionFilter : public Filter {
private:
    std::string extension;

public:
    ExtensionFilter(const std::string& extension) : extension(extension) {}

    std::vector<File> meetCriteria(const std::vector<File>& files) const override {
        std::vector<File> filteredFiles;
        for (const auto& file : files) {
            if (file.getExtension() == extension) {
                filteredFiles.push_back(file);
            }
        }
        return filteredFiles;
    }
};

class NameFilter : public Filter {
private:
    std::string name;

public:
    NameFilter(const std::string& name) : name(name) {}

    std::vector<File> meetCriteria(const std::vector<File>& files) const override {
        std::vector<File> filteredFiles;
        for (const auto& file : files) {
            if (file.getName() == name) {
                filteredFiles.push_back(file);
            }
        }
        return filteredFiles;
    }
};

class SizeFilter : public Filter {
private:
    int size;

public:
    SizeFilter(int size) : size(size) {}

    std::vector<File> meetCriteria(const std::vector<File>& files) const override {
        std::vector<File> filteredFiles;
        for (const auto& file : files) {
            if (file.getSize() == size) {
                filteredFiles.push_back(file);
            }
        }
        return filteredFiles;
    }
};

class AndFilter : public Filter {
private:
    Filter* filter1;
    Filter* filter2;

public:
    AndFilter(Filter* filter1, Filter* filter2) : filter1(filter1), filter2(filter2) {}

    std::vector<File> meetCriteria(const std::vector<File>& files) const override {
        std::vector<File> firstCriteriaFiles = filter1->meetCriteria(files);
        return filter2->meetCriteria(firstCriteriaFiles);
    }
};

int main() {
    std::vector<File> files = {
        File("file1", "txt", 100),
        File("file2", "pdf", 200),
        File("file3", "txt", 150),
        File("file4", "doc", 250),
        File("file5", "txt", 100)
    };

    // Create individual filters
    Filter* txtFilter = new ExtensionFilter("txt");
    Filter* sizeFilter = new SizeFilter(100);
    Filter* nameFilter = new NameFilter("file3");

    // Combine filters
    Filter* andFilter = new AndFilter(txtFilter, sizeFilter);

    // Apply filters
    std::vector<File> txtFiles = txtFilter->meetCriteria(files);
    std::vector<File> txtAndSizeFiles = andFilter->meetCriteria(files);

    return 0;
}


// pick a sorting algorithm during runtime

////////////////////////////////
// Strategy   /////////////////
///////////////////////////////

class SortStrategy {
    public:
        virtual void sort(std::vector<int>& data) = 0;
};

class BubbleSort: public SortStrategy {
    public:
        void sort(std::vector<int>&data) override {
            std::cout<<"do bubble sort";
        }
};

class QuickSort: public SortStrategy {
    public:
        void sort(std::vector<int>&data) override {
            std::cout<<"do quick sort";
        }
};

class MergeSort: public SortStrategy {
    public:
        void sort(std::vector<int>&data) override {
            std::cout<<"do merge sort";
        }
};

class Sorter {
    private:
        SortStrategy* sortstartegy;
    public:
        Sorter(SortStrategy* strategy): sortstartegy(strategy) {}

        void sort(std::vector<int>&data) {
            sortstartegy->sort(data);
        }
};

int main() {
    std::vector<int> vec = {5,4,2,1};
    SortStrategy* bubble = new BubbleSort();
    Sorter* sorter = new Sorter(bubble);
    sorter->sort(vec);
}


// Let’s say we have a weather station that monitors temperature, humidity, 
// and pressure. Different display elements (e.g., Current Conditions, Statistics, Forecast)
// need to be updated whenever the weather data changes.

////////////////////////////////
// Observer   /////////////////
///////////////////////////////

class Observer {
    virtual void update(int temp,int humidity,int pressure) = 0;
};

class Subject {
    virtual void registerObserver(Observer* o) = 0;
    virtual void removeObserver(Observer* o) = 0;
    virtual void notifyAll() = 0;
};

class Weatherdata: public Subject {
    private:
        int temp;
        int humid;
        int pres;
        std::vector<Observer* > observers;

    public:
        void registerObserver(Observer* ob) override {
            observers.push_back(ob);
        }

        void removeObserver(Observer* ob) override {
            auto it = std::remove(observers.begin(),observers.end(),ob);
            observers.erase(it, observers.end());
        }

        void notifyAll() override {
            for(auto ob: observers)  {
                ob->update(temp, humid, pres);
            }
        }

        void setMeasurements(int temp, int humid, int pres) {
            temp = temp;
            humid = humid;
            pres = pres;
            notifyAll();
        }
};

class CurrentConditionDisplay: public Observer {
    private:
        int temp;
        int humid;
    public:
        void update(int temp,int humidity,int pressure) override {
            temp = temp;
            humid = humidity;
            display();
        }

        void display() {
            std::cout<<"temp and humidity updated in display";
        }

};

class ForeCast: public Observer {
    private:
        int temp;
        int pres;
    public:
        void update(int temp,int humidity,int pressure) override {
            temp = temp;
            pres = pressure;
            display();
        }

        void display() {
            std::cout<<"temp and pressure updated in display";
        }

};

int main() {
    Weatherdata* wd = new Weatherdata();
    CurrentConditionDisplay* ccd = new CurrentConditionDisplay();
    ForeCast* fc = new ForeCast();
    wd->registerObserver(ccd);
    wd->registerObserver(fc);
    wd->setMeasurements(1,2,3);
    wd->setMeasurements(2,3,4);
}


// Let’s consider a scenario where we want to create a system to prepare different types 
// of beverages, such as tea and coffee. The preparation 
// process for these beverages is similar, but there are slight differences in the steps.

////////////////////////////////
// Template   /////////////////
///////////////////////////////

class CaffeineBeverage {
public:
    void prepareRecipe() {
        boilWater();
        brew();
        pourInCup();
        addCondiments();
    }

protected:
    void boilWater() {
        std::cout << "Boiling water\n";
    }

    void pourInCup() {
        std::cout << "Pouring into cup\n";
    }

    // Steps that must be implemented by subclasses
    virtual void brew() = 0;
    virtual void addCondiments() = 0;
};

class Tea : public CaffeineBeverage {
protected:
    void brew() override {
        std::cout << "Steeping the tea\n";
    }

    void addCondiments() override {
        std::cout << "Adding lemon\n";
    }
};


class Coffee : public CaffeineBeverage {
protected:
    void brew() override {
        std::cout << "Dripping coffee through filter\n";
    }

    void addCondiments() override {
        std::cout << "Adding sugar and milk\n";
    }
};

int main() {
    Tea myTea;
    Coffee myCoffee;

    std::cout << "Making tea...\n";
    myTea.prepareRecipe();

    std::cout << "\nMaking coffee...\n";
    myCoffee.prepareRecipe();

    return 0;
}


// Let’s consider a scenario where a coffee shop sells different types of coffee 
// with various add-ons like milk, soy, mocha, etc. 
// Each add-on can be added dynamically, 
// and the final price depends on the base coffee and the add-ons.

////////////////////////////////
// Decorator   ////////////////
///////////////////////////////

class Beverage {
    public:
        virtual std::string getDescription() = 0;
        virtual double getCost() = 0; 
};

class Espresso: public Beverage {
    public:
        std::string getDescription() override {
            return "espresso";
        }

        double getCost() override {
            return 2.0;
        }
};

class Espresso: public Beverage {
    public:
        std::string getDescription() override {
            return "mocha";
        }

        double getCost() override {
            return 3.0;
        }
};

class CondimentDecorator: public Beverage {
    private:
        Beverage* bev;
    public:
        CondimentDecorator(Beverage* beverage): bev(beverage){}
};

class Milk: public CondimentDecorator {
    public:
        Milk(Beverage* bev): CondimentDecorator(bev){}
        std::string getDescription() override {
            return bev->getDescription() + "Milk";
        }

        double getCost() override {
            return bev->getCost()+ 1.0;
        }
};

class Soya: public CondimentDecorator {
    public:
        Soya(Beverage* bev): CondimentDecorator(bev){}
        std::string getDescription() override {
            return bev->getDescription() + "Soya";
        }

        double getCost() override {
            return bev->getCost()+ 1.2;
        }
}; 

int main() {
    Beverage* bev = new Espresso();
    bev = new Milk(bev);
    bev = new Soya(bev);
    std::cout<<bev->getDescription()<<" "<<bev->getCost()<<std::endl;
}

////////////////////////////////
// Adaptar     ////////////////
///////////////////////////////

interface Lion
{
    public function roar();
}

class AfricanLion implements Lion
{
    public function roar()
    {
    }
}

class AsianLion implements Lion
{
    public function roar()
    {
    }
}

class Hunter
{
    public function hunt(Lion $lion)
    {
        $lion->roar();
    }
}

// This needs to be added to the game
class WildDog
{
    public function bark()
    {
    }
}

// Adapter around wild dog to make it compatible with our game
class WildDogAdapter implements Lion
{
    protected $dog;

    public function __construct(WildDog $dog)
    {
        $this->dog = $dog;
    }

    public function roar()
    {
        $this->dog->bark();
    }
}

$wildDog = new WildDog();
$wildDogAdapter = new WildDogAdapter($wildDog);

$hunter = new Hunter();
$hunter->hunt($wildDogAdapter);



// We want to design a meeting booking scheduler that allows users to book 
// different types of meetings. The system should handle various meeting types, 
// such as one-on-one meetings, group meetings, and recurring meetings. 
// Each meeting type may require a different setup process, 
// but all share a common interface for scheduling.

////////////////////////////////
// Factory     ////////////////
///////////////////////////////

class MeetingF {
    public:
        virtual void schedule() = 0;
};

class OneonOneMeeting: public MeetingF {
    public:
        void schedule() override {
            std::cout<<"Schedule an one on one meeting";
        }
};

class GroupMeeting: public MeetingF {
    public:
        void schedule() override {
            std::cout<<"Schedule a group meeting";
        }
};

class MeetingScheduler {
    public:
        virtual MeetingF* createMeeting() = 0;
        void bookMeeting() {
            MeetingF* meeting = createMeeting();
            meeting->schedule();
        }
};

class OneOnOneMeetingScheduler: public MeetingScheduler {
    public:
        MeetingF* createMeeting() override {
            return new OneonOneMeeting(); 
        }
};

class GroupMeetingScheduler: public MeetingScheduler {
    public:
        MeetingF* createMeeting() override {
            return new GroupMeeting(); 
        }
};

int main() {
    MeetingScheduler* oneOnemeeting = new OneOnOneMeetingScheduler();
    oneOnemeeting->bookMeeting();
}

// create meeting schedulers with option to select time range
////////////////////////////////
// Builder     ////////////////
///////////////////////////////

class Meeting {
private:
    std::string type;
    std::string from_time;
    std::string to_time;
    int participants;

public:
    // Constructor is private to enforce creation via Builder
    Meeting(const std::string& type, const std::string& from_time, const std::string& to_time, int participants)
        : type(type), from_time(from_time), to_time(to_time), participants(participants) {}

    void displayDetails() const {
        std::cout << "Meeting Type: " << type << "\n";
        std::cout << "From: " << from_time << "\n";
        std::cout << "To: " << to_time << "\n";
        std::cout << "Participants: " << participants << "\n";
    }
};

class MeetingBuilder {
private:
    std::string type;
    std::string from_time;
    std::string to_time;
    int participants;

public:
    MeetingBuilder& setType(const std::string& meetingType) {
        type = meetingType;
        return *this;
    }

    MeetingBuilder& setFromTime(const std::string& fromTime) {
        from_time = fromTime;
        return *this;
    }

    MeetingBuilder& setToTime(const std::string& toTime) {
        to_time = toTime;
        return *this;
    }

    MeetingBuilder& setParticipants(int numberOfParticipants) {
        participants = numberOfParticipants;
        return *this;
    }

    Meeting build() const {
        return Meeting(type, from_time, to_time, participants);
    }
};

int main() {
    // Create a one-on-one meeting from 10:00 AM to 11:00 AM with 2 participants
    MeetingBuilder builder;
    Meeting oneOnOneMeeting = builder.setType("One-on-One")
                                      .setFromTime("10:00 AM")
                                      .setToTime("11:00 AM")
                                      .setParticipants(2)
                                      .build();

    oneOnOneMeeting.displayDetails();

    // Create a group meeting from 2:00 PM to 3:00 PM with 5 participants
    Meeting groupMeeting = builder.setType("Group")
                                  .setFromTime("2:00 PM")
                                  .setToTime("3:00 PM")
                                  .setParticipants(5)
                                  .build();

    groupMeeting.displayDetails();

    return 0;
}




/////////////// Parking Lot Example /////////////

class Vehicle {
protected:
    std::string licensePlate;
    VehicleSize size;

public:
    Vehicle(std::string licensePlate, VehicleSize size);

    VehicleSize getSize() const;
    std::string getLicensePlate() const;
};


typedef enum {
Small,
Compact
} VehicleSize;

class ParkingLot {
private:
    std::vector<ParkingLevel> levels;

public:
    ParkingLot(int numLevels, int numSpotsPerLevel);

    ParkingSpot* findAvailableSpot(Vehicle* vehicle);
    void parkVehicle(Vehicle* vehicle, ParkingSpot* spot);
    void freeSpot(ParkingSpot* spot);
};


class ParkingSpotAllocator {
public:
    ParkingSpot* findSpot(ParkingLot* lot, Vehicle* vehicle) {
        // Get the list of parking levels in the parking lot
        std::vector<ParkingLevel>& levels = lot->getLevels();

        // Iterate over each level in the parking lot
        for (ParkingLevel& level : levels) {
            // Try to find an available spot for the vehicle on the current level
            ParkingSpot* spot = level.findAvailableSpot(vehicle);

            // If a suitable spot is found, return it
            if (spot != nullptr) {
                return spot;
            }
        }

        // If no suitable spot is found on any level, return nullptr
        return nullptr;
    }
};

class ParkingLevel {
private:
    int levelNumber;
    std::vector<ParkingSpot> spots;

public:
    ParkingLevel(int levelNumber, int numSpots) : levelNumber(levelNumber) {
        // Initialize parking spots
        for (int i = 0; i < numSpots; ++i) {
            spots.push_back(ParkingSpot(i + 1, Compact)); // Example with all spots compact
        }
    }

    ParkingSpot* findAvailableSpot(Vehicle* vehicle) {
        for (ParkingSpot& spot : spots) {
            if (spot.canFitVehicle(vehicle) && spot.getAvailability()) {
                return &spot; // Found an available spot that can fit the vehicle
            }
        }
        return nullptr; // No suitable spot found on this level
    }

    void freeSpot(int spotNumber) {
        spots[spotNumber - 1].freeSpot();
    }
};

class ParkingSpot {
private:
    int spotNumber;
    bool isAvailable;
    VehicleSize size;
    Vehicle* currentVehicle;

public:
    ParkingSpot(int spotNumber, VehicleSize size) 
        : spotNumber(spotNumber), size(size), isAvailable(true), currentVehicle(nullptr) {}

    bool canFitVehicle(Vehicle* vehicle) {
        return vehicle->getSize() <= size;
    }

    void parkVehicle(Vehicle* vehicle) {
        currentVehicle = vehicle;
        isAvailable = false;
    }

    void freeSpot() {
        currentVehicle = nullptr;
        isAvailable = true;
    }

    bool getAvailability() const {
        return isAvailable;
    }

    int getSpotNumber() const {
        return spotNumber;
    }
};

class FeeStrategy {
public:
    virtual ~FeeStrategy() = default;
    virtual double calculateFee(std::time_t entryTime, std::time_t exitTime, VehicleSize size) const = 0;
};

class HourlyRateStrategy : public FeeStrategy {
private:
    double hourlyRate;

public:
    HourlyRateStrategy(double rate) : hourlyRate(rate) {}

    double calculateFee(std::time_t entryTime, std::time_t exitTime, VehicleSize size) const override {
        double durationInHours = std::difftime(exitTime, entryTime) / 3600.0;
        return hourlyRate * std::ceil(durationInHours);
    }
};

class FlatRateStrategy : public FeeStrategy {
private:
    double flatRate;

public:
    FlatRateStrategy(double rate) : flatRate(rate) {}

    double calculateFee(std::time_t entryTime, std::time_t exitTime, VehicleSize size) const override {
        return flatRate;
    }
};

class ParkingFeeCalculator {
private:
    FeeStrategy* feeStrategy;

public:
    ParkingFeeCalculator(FeeStrategy* strategy) : feeStrategy(strategy) {}

    void setStrategy(FeeStrategy* strategy) {
        feeStrategy = strategy;
    }

    double calculateFee(ParkingTicket* ticket, std::time_t exitTime) const {
        return feeStrategy->calculateFee(ticket->getEntryTime(), exitTime, ticket->getVehicleSize());
    }
};

