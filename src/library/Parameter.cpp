#include "Parameter.h"

    

    EnumParameter::EnumParameter(std::string name, std::vector<std::string> options, int default_value)
    {
        this->name = name;
        this->options = options;
        this->value = default_value;
        this->default_value = default_value;
    }

    EnumParameter::~EnumParameter()
    {
        std::cout << "Parameter destroyed\n";
    }

    // Set the value of the parameter
    void EnumParameter::set_value(int _value)
    {
        if (this->value != _value)
        {
            this->value = _value;
            if (on_change)
            {
                on_change(this->value);
            }
            notify(EnumParameter::Events::VALUE, this->value);
        }
    }

    void EnumParameter::set_option(std::string option)
    {
        auto it = std::find(options.begin(), options.end(), option);
        if (it != options.end())
        {
            set_value(std::distance(options.begin(), it));
        }
    }

    // Get the value of the parameter
    int EnumParameter::get_value()
    {
        return value;
    }

    std::string EnumParameter::get_option()
    {
        return options[value];
    }

    // Increment the value of the parameter
    void EnumParameter::increment()
    {
        // Increment the value of the parameter, loop back to 0 if max is reached
        if (value == options.size() - 1)
        {
            set_value(0);
        }
        else
        {
            set_value(value + 1);
        }
    }

    // Decrement the value of the parameter
    void EnumParameter::decrement()
    {
        // Decrement the value of the parameter, loop back to max if 0 is reached
        if (value == 0)
        {
            set_value(options.size() - 1);
        }
        else
        {
            set_value(value - 1);
        }
    }

    // Update listeners
    void EnumParameter::update()
    {
        if (on_change)
        {
            on_change(this->value);
        }
        this->notify(EnumParameter::Events::VALUE, this->value);
    }

    void EnumParameter::encode(YAML::Emitter &out)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << name;
        out << YAML::Key << "value" << YAML::Value << value;
        out << YAML::Key << "default_value" << YAML::Value << default_value;
        out << YAML::Key << "options" << YAML::Value << YAML::BeginSeq;
        for (auto option : options)
        {
            out << option;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
    }

    void EnumParameter::decode(YAML::Node &node)
    {
        value = node["value"].as<int>();
        default_value = node["default_value"].as<int>();
        options.clear();
        for (auto option : node["options"])
        {
            options.push_back(option.as<std::string>());
        }
    }   

