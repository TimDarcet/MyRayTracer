#pragma once

#include <algorithm>
#include <vector>
#include <assert.h>
#include <unordered_map>
#include "Vec3.h"
#include "AABB.h"

using namespace std;

class BVH {
    public:
        AABB m_aabb;
        BVH *m_child_1;
        BVH *m_child_2;
        vector<Triangle *> *m_triangles;
        int m_cut_axis;
        float m_cut;

        BVH(AABB aabb=AABB(), BVH *child_1=NULL, BVH *child_2=NULL, vector<Triangle *> *triangles=new vector<Triangle *>(), int cut_axis=0, float cut=0) {
            this->m_aabb = aabb;
            this->m_child_1 = child_1;
            this->m_child_2 = child_2;
            this->m_triangles = triangles;
            this->m_cut_axis = cut_axis;
            this->m_cut = cut;
        }

        ~BVH() {
            delete this->m_triangles;
            if (this->m_child_1 != NULL) {
                delete this->m_child_1;
            }
            if (this->m_child_2 != NULL) {
                delete this->m_child_2;
            }
        }

        void check_cut_axis() const {
            assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            if (m_child_1 != NULL) {
                m_child_1->check_cut_axis();
            }
            if (m_child_2 != NULL) {
                m_child_2->check_cut_axis();
            }
        }

        void from_triangles(vector<Triangle *> *build_triangles) {
            assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2 && !(build_triangles->empty()));
            // Get m_triangles
            this->m_triangles = new vector<Triangle *>(*build_triangles);
            // this->m_triangles = *;
            // Calculate m_aabb
            for (Triangle *t: *build_triangles) {
                for (Vertex *v: t->m_vertices) {
                    for (int i=0; i<3; i++) {
                        this->m_aabb.m_p1[i] = min(this->m_aabb.m_p1[i], v->m_point[i] - __FLT_EPSILON__ * 10);
                        this->m_aabb.m_p2[i] = max(this->m_aabb.m_p2[i], v->m_point[i] + __FLT_EPSILON__ * 10);
                    }
                }
            }
            // Calculate max vertex degree
            assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            unordered_map<Vertex *, int> degrees;
            for (Triangle *t: *build_triangles) {
                for (Vertex *v: t->m_vertices) {
                    if (degrees.find(v) != degrees.end()) {
                        degrees[v]++;
                    }
                    else
                        degrees[v] = 1;
                }
            }
            using pair_type = decltype(degrees)::value_type;
            int max_degree = max_element(degrees.begin(),
                                         degrees.end(),
                                         [] (const pair_type & p1, const pair_type & p2) {return p1.second < p2.second;}
                                        )->second;
            assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            // Calculate children
            if (this->m_triangles->size() <= max_degree + 1) {
                this->m_cut_axis = 0;
                this->m_child_1 = NULL;
                this->m_child_2 = NULL;
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                return;
            }
            else {
                // Calculate cut axis
                int longidx;
                float longlength = 0;
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                for (int i=0; i<3; i++) {
                    float lengthi = this->m_aabb.m_p2[i] - this->m_aabb.m_p1[i];
                    if (lengthi > longlength) {
                        longlength = lengthi;
                        longidx = i;
                    }
                }
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                this->m_cut_axis = longidx;
                // Calculate list of coordinates
                // vector<float> min_coords;
                // vector<float> max_coords;
                vector<float> coords;
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                for (Triangle *t: *build_triangles) {
                    Vec3f p0 = t->m_vertices[0]->m_point;
                    Vec3f p1 = t->m_vertices[1]->m_point;
                    Vec3f p2 = t->m_vertices[2]->m_point;
                    coords.push_back((p0[m_cut_axis] + p1[m_cut_axis] + p2[m_cut_axis]) / 3);
                    // min_coords.push_back(min(p0[m_cut_axis], min(p1[m_cut_axis], p2[m_cut_axis])));
                    // max_coords.push_back(max(p0[m_cut_axis], max(p1[m_cut_axis], p2[m_cut_axis])));
                }
                // Sort coords
                // using pair_type = decltype(*build_triangles)::value_type;
                typedef bool (*comp) (Triangle *t1, Triangle *t2);
                comp comparators[] = {
                    [](Triangle *t1, Triangle *t2) {
                        return t1->m_vertices[0]->m_point[0] + t1->m_vertices[1]->m_point[0] + t1->m_vertices[2]->m_point[0]
                             < t2->m_vertices[0]->m_point[0] + t2->m_vertices[1]->m_point[0] + t2->m_vertices[2]->m_point[0];
                    },
                    [](Triangle *t1, Triangle *t2) {
                        return t1->m_vertices[0]->m_point[1] + t1->m_vertices[1]->m_point[1] + t1->m_vertices[2]->m_point[1]
                             < t2->m_vertices[0]->m_point[1] + t2->m_vertices[1]->m_point[1] + t2->m_vertices[2]->m_point[1];
                    },
                    [](Triangle *t1, Triangle *t2) {
                        return t1->m_vertices[0]->m_point[2] + t1->m_vertices[1]->m_point[2] + t1->m_vertices[2]->m_point[2]
                             < t2->m_vertices[0]->m_point[2] + t2->m_vertices[1]->m_point[2] + t2->m_vertices[2]->m_point[2];
                    }
                };
                sort(
                    build_triangles->begin(),
                    build_triangles->end(),
                    comparators[this->m_cut_axis]
                );
                // sort(min_coords.begin(), min_coords.end());
                // sort(max_coords.begin(), max_coords.end());
                // Calculate best cut
                // Takes cut that minimizes the max of the sizes of the two sublists
                // int i = 0;
                // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                // int j = 0;
                // float best_cut;
                // int best_m = min_coords.size();
                // while (i < best_m) {
                //     if ((i > 0 && abs(min_coords[i] - min_coords[i - 1]) < 2 * __FLT_EPSILON__)
                //         || (j > 0 && abs(max_coords[j] - max_coords[j - 1]) < 2 * __FLT_EPSILON__)) {
                //         // can't take a cut here
                //     }
                //     else {
                //         int here_m = max(i, (int)max_coords.size() - j);
                //         if (here_m < best_m) {
                //             best_m = here_m;
                //             assert(j > 0 && i > 0);
                //             if (min_coords[i] < max_coords[j]) {
                //                 assert(max(min_coords[i - 1], max_coords[j - 1]) < min_coords[i]);
                //                 best_cut = (max(min_coords[i - 1], max_coords[j - 1]) + min_coords[i]) / 2;
                //             }
                //             else if (min_coords[i] > max_coords[j]) {
                //                 assert(max(min_coords[i - 1], max_coords[j - 1]) < max_coords[j]);
                //                 best_cut = (max(min_coords[i - 1], max_coords[j - 1]) + max_coords[j]) / 2;
                //             }
                //             else {
                //                 best_cut = min_coords[i] - __FLT_EPSILON__;
                //                 assert (best_cut > min_coords[i - 1]);
                //                 assert (best_cut > max_coords[j - 1]);
                //             }
                //         }
                //     }
                //     if (min_coords[i] <= max_coords[j]) {
                //         i++;
                //     }
                //     else {
                //         j++;
                //     }
                // }
                // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                // if (best_m >= this->m_triangles.size()) {
                //     this-> m_child_1 = NULL;
                //     this-> m_child_2 = NULL;
                //     assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                //     return;
                // }
                // this->m_cut = best_cut;

                vector<Triangle *> triangles_1(build_triangles->begin(), build_triangles->begin() + build_triangles->size() / 2);
                vector<Triangle *> triangles_2(build_triangles->begin() + build_triangles->size() / 2 + 1, build_triangles->end());
                // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                // for (Triangle *t: this->m_triangles) {
                //     bool in_1 = false;
                //     bool in_2 = false;
                //     for (int i=0; i<3; i++) {
                //         if (t->m_vertices[i]->m_point[this->m_cut_axis] <= this->m_cut) {
                //             in_1 = true;
                //         }
                //         if (t->m_vertices[i]->m_point[this->m_cut_axis] >= this->m_cut)
                //             in_2 = true;
                //     }
                //     if (in_1)
                //         triangles_1.push_back(t);
                //     if (in_2)
                //         triangles_2.push_back(t);
                // }
                // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                BVH *child_1 = new BVH();
                this->m_child_1 = child_1;
                // this->m_child_1->m_aabb = AABB(this->m_aabb->m_p1, this->m_aabb->m_p2) // TODO: initial AABB
                this->m_child_1->from_triangles(&triangles_1);
                BVH *child_2 = new BVH();
                this->m_child_2 = child_2;
                this->m_child_2->from_triangles(&triangles_2);
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            }
            this->check_cut_axis();
        }

        vector<float> intersection(Ray r, Triangle &t) const {
            this->check_cut_axis();
            Vec3f entry;
            Vec3f exit;
            // cout << "test1" << endl;
            if (this->m_aabb.ray_intersection(r, entry, exit)) {
                // cout << "test2" << endl;
                if (this->m_child_1 == NULL) {
                    // cout << "test3" << endl;
                    assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                    vector<float> best_inter = vector<float>();
                    for (auto t_here : *this->m_triangles) {
                        vector<float> this_inter = r.intersect(*t_here);
                        if (this_inter.size() > 0 && (best_inter.size() == 0 || best_inter[3] > this_inter[3])) {
                            best_inter = this_inter;
                            t = *t_here;
                        }
                    }
                    // cout << "test4" << endl;
                    this->check_cut_axis();
                    return best_inter;
                }
                else {
                    vector<float> this_inter = vector<float>();
                    assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                    // cout << "test8 " << this->m_cut_axis << " " << entry << " " << this->m_child_1 << endl;
                    if (entry[this->m_cut_axis] <= this->m_cut || exit[this->m_cut_axis] <= this->m_cut) {
                        // cout << "test5" << endl;
                        this_inter = this->m_child_1->intersection(r, t);
                    }
                    if (entry[this->m_cut_axis] >= this->m_cut || exit[this->m_cut_axis] >= this->m_cut) {
                        // cout << "test6" << endl;
                        Triangle new_t = Triangle(NULL, NULL, NULL);
                        vector<float> new_inter = this->m_child_2->intersection(r, new_t);
                        if (this_inter.size() == 0 || (new_inter.size() > 0 && this_inter[3] > new_inter[3])) {
                            // cout << "test7" << endl;
                            this_inter = new_inter;
                            t = new_t;
                        }
                    }
                    return this_inter;
                }
            }
            else {
                // cout << "test9" << endl;
                return vector<float>();
            }
        }
};
