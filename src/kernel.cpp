#include "include/paging.h"
#include "include/tasking.h"
#include "include/process/process_manager.h"
#include "include/shell.h"
#include "include/keyboard.h"
#include "include/vectors.h"
#include "include/io.h"
#include "interrupts.h"

int getAge()
{
    vector<char> inputStr;
    print_string("Enter age: ");
    input(inputStr);

    // Convert string to integer
    int age = 0;
    for (size_t i = 0; i < inputStr.size(); i++)
    {
        if (inputStr[i] >= '0' && inputStr[i] <= '9')
        {
            age = age * 10 + (inputStr[i] - '0');
        }
    }
    return age;
}

vector<char> getFirstName()
{
    vector<char> firstName;
    print_string("Enter first name: ");
    input(firstName);
    print_char('\n');
    return firstName;
}

vector<char> getLastName()
{
    vector<char> lastName;
    print_string("Enter last name: ");
    input(lastName);
    print_char('\n');
    return lastName;
}

vector<char> calculateLifeStage(int age)
{
    if (age >= 0 && age <= 2)
    {
        return to_vector("Infancy");
    }
    else if (age <= 4)
    {
        return to_vector("Toddlerhood");
    }
    else if (age <= 8)
    {
        return to_vector("Early Childhood");
    }
    else if (age <= 12)
    {
        return to_vector("Middle Childhood");
    }
    else if (age <= 19)
    {
        return to_vector("Adolescence");
    }
    else if (age <= 40)
    {
        return to_vector("Early Adulthood");
    }
    else if (age <= 65)
    {
        return to_vector("Middle Adulthood");
    }
    else
    {
        return to_vector("Late Adulthood");
    }
}

extern "C" void kernel_main()
{
    // Initialize shell first for debug output
    Kernel::Shell::initialize();
    Kernel::Shell::print("Initializing core systems...\n");

    // Initialize interrupts
    Kernel::Shell::print("Setting up interrupt handling...\n");
    Kernel::InterruptManager::initialize();

    // Initialize paging
    Kernel::Shell::print("Setting up paging...\n");
    setup_paging();

    // Initialize process management
    Kernel::Shell::print("Initializing process manager...\n");
    Kernel::ProcessManager::initialize();

    // Initialize tasking
    Kernel::Shell::print("Setting up task scheduler...\n");
    init_tasking();

    // Initialize keyboard
    Kernel::Shell::print("Initializing keyboard...\n");
    Kernel::Keyboard::initialize();

    Kernel::Shell::print("Enabling interrupts...\n");
    asm volatile("sti");

    Kernel::Shell::print("System initialization complete.\n");

    // Get user input and process
    vector<char> firstName = getFirstName();
    vector<char> lastName = getLastName();
    int age = getAge();

    vector<char> lifeStage = calculateLifeStage(age);
    vector<char> space = to_vector(" ");
    vector<char> stageHandle = to_vector(" is in the stage of ");
    vector<char> combinedName = concat({firstName, space, lastName});
    vector<char> result = concat({combinedName, stageHandle, lifeStage});

    print_vector(result);
    print_char('\n');

    Kernel::Shell::print("$ ");

    // Main kernel loop
    while (1)
    {
        if (Kernel::Keyboard::is_key_available())
        {
            char c = Kernel::Keyboard::get_char();
            if (c != 0)
            {
                Kernel::Shell::handle_input(c);
            }
        }
        asm volatile("hlt");
    }
}