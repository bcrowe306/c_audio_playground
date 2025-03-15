#pragma once
#ifndef OBSERVER_H
#define OBSERVER_H

#include <iostream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <any>

using std::string;
using std::vector;
using std::unordered_map;
using std::any;
using std::shared_ptr;
using std::make_shared;
using std::function;


typedef function<void(string, any)> ObserverFunction;

// forward declaration
class IObserver;
class ISubject : public std::enable_shared_from_this<ISubject>
{
    public:
        ISubject(){};
        virtual ~ISubject(){};
        void attach(string subject_type, shared_ptr<ISubject> subject, shared_ptr<IObserver> observer);
        void detach(string subject_type, shared_ptr<IObserver> observer);
        void notify(string type, any data);

    private:
        unordered_map<string, vector<shared_ptr<IObserver>>> _observer_map;
};


struct SubjectDefinition
{
    shared_ptr<ISubject> subject;
    string subject_type;
};


class IObserver : public std::enable_shared_from_this<IObserver>
{
public:
    IObserver(){};
    // Destructor: Remove the observer from all subjects
    virtual ~IObserver(){
        for (auto subject : _subjects)
        {
            subject.subject->detach(subject.subject_type, shared_from_this());
        }
    };

    // Attach the observer to a subject
    void track_subjects(shared_ptr<ISubject> subject, string subject_type){
        _subjects.push_back({subject, subject_type});
    }

    // virtual void update(std::any data) = 0;
    void update(string subject_type, any data){
        // Find the list of functions by subject type
        auto it = _functions_map.find(subject_type);
        if (it != _functions_map.end())
        {
            // If the list of functions is found, iterate through the list and call each function
            for (auto function : it->second)
            {
                function(subject_type, data);
            }
        }
    };

    // Bind a function to a subject type. This appends the function to the list of functions to be called when the subject_type is updated.
    void bind_function(string subject_type, ObserverFunction function){
        // Check if the subject type already exists in the map
        if (_functions_map.find(subject_type) == _functions_map.end())
        {
            // If the subject type does not exist, create a new vector and add the function
            _functions_map[subject_type] = {function};
        }
        else
        {
            _functions_map[subject_type].push_back(function);
        }
    }


protected:
    vector<SubjectDefinition> _subjects;
    unordered_map<string, vector<ObserverFunction>> _functions_map;
};




#endif // !OBSERVER_H
