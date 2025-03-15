#include "Observer.h"
void ISubject::attach(string subject_type, shared_ptr<ISubject> subject, shared_ptr<IObserver> observer)
{
    // Add the observer to the list of observers by subject type
    // need to check if the subject type already exists in the map
    if (_observer_map.find(subject_type) == _observer_map.end())
    {
        // If the subject type does not exist, create a new vector and add the observer
        _observer_map[subject_type] = {observer};
        observer->track_subjects(subject, subject_type);
    }
    else
    {
        _observer_map[subject_type].push_back(observer);
    }
};

// Detach an observer from the subject by subject type.
void ISubject::detach(string subject_type, shared_ptr<IObserver> observer)
{
    // Find the observer in the list of observers by subject type
    auto it = std::find(_observer_map[subject_type].begin(), _observer_map[subject_type].end(), observer);
    if (it != _observer_map[subject_type].end())
    {
        // If the observer is found, remove it from the list
        _observer_map[subject_type].erase(it);
    }
};
void ISubject::notify(string type, std::any data)
{
    // Find the list of observers by subject type
    auto it = _observer_map.find(type);
    if (it != _observer_map.end())
    {
        // If the list of observers is found, iterate through the list and call the update method on each observer
        for (auto observer : it->second)
        {
            observer->update(type, data);
        }
    }
};