#ifndef hit_polymorph_h
#define hit_polymorph_h

// avoids dynamic dispatch with virtual functions
// include last or after all other type headers
// marginal performance improvement, more time is spent in range checks and math

bool Hittable::hit(const Ray& ray, const Interval& limits, Hit& hit) {
    switch (this->type) {
        case HittableType_BVH_Node:
            return (static_cast<BVH_Node*>(this))->hit(ray, limits, hit);
        case HittableType_List:
            return (static_cast<HittableList*>(this))->hit(ray, limits, hit);
        case HittableType_Translate:
            return (static_cast<Translate*>(this))->hit(ray, limits, hit);
        case HittableType_RotateY:
            return (static_cast<RotateY*>(this))->hit(ray, limits, hit);
        case HittableType_Sphere:
            return (static_cast<Sphere*>(this))->hit(ray, limits, hit);
        case HittableType_Quad:
            return (static_cast<Quad*>(this))->hit(ray, limits, hit);
        case HittableType_ConstantMedium:
            return (static_cast<ConstantMedium*>(this))->hit(ray, limits, hit);
    }
}

AABB Hittable::bounding_box() {
    switch (this->type) {
        case HittableType_BVH_Node:
            return (static_cast<BVH_Node*>(this))->bounding_box();
        case HittableType_List:
            return (static_cast<HittableList*>(this))->bounding_box();
        case HittableType_Translate:
            return (static_cast<Translate*>(this))->bounding_box();
        case HittableType_RotateY:
            return (static_cast<RotateY*>(this))->bounding_box();
        case HittableType_Sphere:
            return (static_cast<Sphere*>(this))->bounding_box();
        case HittableType_Quad:
            return (static_cast<Quad*>(this))->bounding_box();
        case HittableType_ConstantMedium:
            return (static_cast<ConstantMedium*>(this))->bounding_box();
    }
}

#endif /* hit_polymorph_h */
