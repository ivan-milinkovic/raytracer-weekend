#ifndef hittable_list_h
#define hittable_list_h

#include <vector>
#include "aabb.h"
#include "hittable.h"

// Used to group a list of geometry in RotateY and Translate

class HittableList : public Hittable {
  public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList(): Hittable(HittableType_List) {}
    HittableList(shared_ptr<Hittable> object): Hittable(HittableType_List) {
        add(object);
    }
    HittableList(vector<shared_ptr<Hittable>> objects): Hittable(HittableType_List), objects(objects) {
        for (auto obj: objects) {
            add(obj); // add updates the bounding box
        }
    }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = AABB(bbox, object->bounding_box());
    }

    bool hit(const Ray& r, const Interval& limits, Hit& hit) const {
        Hit tmp_hit;
        bool hit_anything = false;
        auto closest_so_far = limits.max;

        for (const auto& object : objects) {
            if (object->hit(r, Interval(limits.min, closest_so_far), tmp_hit)) {
                hit_anything = true;
                closest_so_far = tmp_hit.d;
                hit = tmp_hit;
            }
        }
        return hit_anything;
    }

    AABB bounding_box() const { return bbox; }

  private:
    AABB bbox;
};

#endif /* hittable_list_h */
