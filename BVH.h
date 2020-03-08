#pragma once

#include <algorithm>
#include <vector>
#include <functional>
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
        vector<int> m_triangles;
        int m_cut_axis;
        float m_cut;

        BVH() {
            this->m_aabb = AABB();
            this->m_child_1 = NULL;
            this->m_child_2 = NULL;
            this->m_triangles = vector<int>();
            this->m_cut_axis = 0;
            this->m_cut = 0;
        }

        // Deep copy
        BVH(const BVH &to_copy) {
            if (to_copy.m_child_1 != NULL)
                this->m_child_1 = new BVH(*to_copy.m_child_1);
            else
                this->m_child_1 = NULL;
            BVH *new_child_2;
            if (to_copy.m_child_2 != NULL)
                this->m_child_2 = new BVH(*to_copy.m_child_2);
            else
                this->m_child_2 = NULL;
            this->m_aabb = to_copy.m_aabb;
            this->m_triangles = to_copy.m_triangles;
            this->m_cut_axis = to_copy.m_cut_axis;
            this->m_cut = to_copy.m_cut;
        }

        ~BVH() {
            if (this->m_child_1 != NULL) {
                delete this->m_child_1;
            }
            if (this->m_child_2 != NULL) {
                delete this->m_child_2;
            }
        }

        // Deep copy
        BVH &operator=(const BVH &to_copy) {
            if (this != &to_copy) { // protect against invalid self-assignment
                // 1: allocate new memory and copy the elements
                BVH *new_child_1;
                if (to_copy.m_child_1 != NULL)
                    new_child_1 = new BVH(*to_copy.m_child_1);
                else
                    new_child_1 = NULL;
                BVH *new_child_2;
                if (to_copy.m_child_2 != NULL)
                    new_child_2 = new BVH(*to_copy.m_child_2);
                else
                    new_child_2 = NULL;
                // 2: deallocate old memory
                if (this->m_child_1 != NULL)
                    delete this->m_child_1;
                if (this->m_child_2 != NULL)
                    delete this->m_child_2;
                // 3: assign the new memory to the object
                this->m_child_1 = new_child_1;
                this->m_child_2 = new_child_2;
                this->m_aabb = to_copy.m_aabb;
                this->m_triangles = to_copy.m_triangles;
                this->m_cut_axis = to_copy.m_cut_axis;
                this->m_cut = to_copy.m_cut;
            }
            return *this;
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

        void from_triangles(vector<int>::const_iterator build_t_begin, vector<int>::const_iterator build_t_end, const vector<Vertex> &vertices, const vector<Vec3i> &triangles) {
            assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2 && build_t_begin < build_t_end);
            // Get m_triangles
            // this->m_triangles = build_triangles;
            // this->m_triangles.(build_t_end - build_t_begin);
            this->m_triangles.assign(build_t_begin, build_t_end);
            // Calculate m_aabb
            for (int t_idx: m_triangles) {
                for (int v_idx=0; v_idx<3; v_idx++) {
                    for (int i=0; i<3; i++) {
                        float coord = vertices[triangles[t_idx][v_idx]].m_point[i];
                        this->m_aabb.m_p1[i] = min(this->m_aabb.m_p1[i], coord - __FLT_EPSILON__ * 2);
                        this->m_aabb.m_p2[i] = max(this->m_aabb.m_p2[i], coord + __FLT_EPSILON__ * 2);
                    }
                }
            }
            // Calculate max vertex degree
            // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            // unordered_map<Vertex *, int> degrees;
            // for (Triangle *t: *build_triangles) {
            //     for (Vertex *v: t->m_vertices) {
            //         if (degrees.find(v) != degrees.end()) {
            //             degrees[v]++;
            //         }
            //         else
            //             degrees[v] = 1;
            //     }
            // }
            // using pair_type = decltype(degrees)::value_type;
            // int max_degree = max_element(degrees.begin(),
            //                              degrees.end(),
            //                              [] (const pair_type & p1, const pair_type & p2) {return p1.second < p2.second;}
            //                             )->second;
            assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            // Calculate children
            if (this->m_triangles.size() <= 1) {
                this->m_cut_axis = 0;
                this->m_child_1 = NULL;
                this->m_child_2 = NULL;
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                return;
            }
            else {
                // Calculate cut axis
                float longlength = 0;
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                for (int i=0; i<3; i++) {
                    float lengthi = this->m_aabb.m_p2[i] - this->m_aabb.m_p1[i];
                    if (lengthi > longlength) {
                        longlength = lengthi;
                        this->m_cut_axis = i;
                    }
                }
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                // Calculate list of coordinates
                // vector<float> coords;
                // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                // for (Triangle *t: *build_triangles) {
                //     Vec3f p0 = t->m_vertices[0]->m_point;
                //     Vec3f p1 = t->m_vertices[1]->m_point;
                //     Vec3f p2 = t->m_vertices[2]->m_point;
                //     coords.push_back((p0[m_cut_axis] + p1[m_cut_axis] + p2[m_cut_axis]) / 3);
                //     // min_coords.push_back(min(p0[m_cut_axis], min(p1[m_cut_axis], p2[m_cut_axis])));
                //     // max_coords.push_back(max(p0[m_cut_axis], max(p1[m_cut_axis], p2[m_cut_axis])));
                // }
                // Sort coords
                // using pair_type = decltype(*build_triangles)::value_type;
                // comp comparators[] = {
                //     ,
                //     [](Triangle *t1, Triangle *t2) {
                //         return t1->m_vertices[0]->m_point[1] + t1->m_vertices[1]->m_point[1] + t1->m_vertices[2]->m_point[1]
                //              < t2->m_vertices[0]->m_point[1] + t2->m_vertices[1]->m_point[1] + t2->m_vertices[2]->m_point[1];
                //     },
                //     [](Triangle *t1, Triangle *t2) {
                //         return t1->m_vertices[0]->m_point[2] + t1->m_vertices[1]->m_point[2] + t1->m_vertices[2]->m_point[2]
                //              < t2->m_vertices[0]->m_point[2] + t2->m_vertices[1]->m_point[2] + t2->m_vertices[2]->m_point[2];
                //     }
                // };
                // typedef bool comp (int ti1, int ti2);
                // bool (int ti1, int ti2) comparator = [this, &triangles, &vertices](int ti1, int ti2) {
                std::function<bool(int, int)> comparator = [this, &triangles, &vertices](int ti1, int ti2) {
                        return vertices[triangles[ti1][0]].m_point[m_cut_axis]
                             + vertices[triangles[ti1][1]].m_point[m_cut_axis]
                             + vertices[triangles[ti1][2]].m_point[m_cut_axis]
                             < vertices[triangles[ti2][0]].m_point[m_cut_axis]
                             + vertices[triangles[ti2][1]].m_point[m_cut_axis]
                             + vertices[triangles[ti2][2]].m_point[m_cut_axis];
                };
                sort(
                    this->m_triangles.begin(),
                    this->m_triangles.end(),
                    comparator
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
                // vector<Triangle *> triangles_1(build_triangles->begin(), build_triangles->begin() + build_triangles->size() / 2);
                // vector<Triangle *> triangles_2(build_triangles->begin() + build_triangles->size() / 2 + 1, build_triangles->end());
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
                this->m_child_1 = new BVH();
                // this->m_child_1->m_aabb = AABB(this->m_aabb->m_p1, this->m_aabb->m_p2) // TODO: initial AABB
                this->m_child_1->from_triangles(
                    m_triangles.begin(),
                    m_triangles.begin() + m_triangles.size() / 2,
                    vertices,
                    triangles
                );
                this->m_child_2 = new BVH();
                this->m_child_2->from_triangles(
                    m_triangles.begin() + m_triangles.size() / 2,
                    m_triangles.end(),
                    vertices,
                    triangles
                );
                assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
            }
            this->check_cut_axis();
        }

        vector<float> intersection(Ray &r, Vec3i &t, const vector<Vertex> &vertices, const vector<Vec3i> &triangles) const {
            this->check_cut_axis();
            Vec3f entry;
            Vec3f exit;
            // cout << "test1" << endl;
            if (this->m_aabb.ray_intersection(r, entry, exit)) {
                // cout << "test2" << endl;
                if (this->m_child_1 == NULL) {
                    cout << "Found the end" << endl;
                    // cout << "test3" << endl;
                    assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                    vector<float> best_inter = vector<float>();
                    for (auto t_idx : this->m_triangles) {
                        Vec3f p0 = vertices[triangles[t_idx][0]].m_point;
                        Vec3f p1 = vertices[triangles[t_idx][1]].m_point;
                        Vec3f p2 = vertices[triangles[t_idx][2]].m_point;
                        vector<float> this_inter = r.intersect(p0, p1, p2);
                        if (this_inter.size() > 0 && (best_inter.size() == 0 || best_inter[3] > this_inter[3])) {
                            best_inter = this_inter;
                            t = triangles[t_idx];
                        }
                    }
                    // cout << "test4" << endl;
                    this->check_cut_axis();
                    return best_inter;
                }
                else {
                    cout << "Deeper" << endl;
                    Vec3i t2;
                    vector<float> inter1 = this->m_child_1->intersection(r, t, vertices, triangles);
                    vector<float> inter2 = this->m_child_2->intersection(r, t2, vertices, triangles);
                    // assert(0 <= this->m_cut_axis && this->m_cut_axis <= 2);
                    // // cout << "test8 " << this->m_cut_axis << " " << entry << " " << this->m_child_1 << endl;
                    // if (entry[this->m_cut_axis] <= this->m_cut || exit[this->m_cut_axis] <= this->m_cut) {
                    //     // cout << "test5" << endl;
                    //     this_inter = this->m_child_1->intersection(r, t, vertices, triangles);
                    // }
                    // if (entry[this->m_cut_axis] >= this->m_cut || exit[this->m_cut_axis] >= this->m_cut) {
                    //     // cout << "test6" << endl;
                    //     Vec3i new_t;
                    //     vector<float> new_inter = this->m_child_2->intersection(r, new_t, vertices, triangles);
                    // }
                    if (inter1.size() == 0 || (inter2.size() > 0 && inter1[3] > inter2[3])) {
                        // cout << "test7" << endl;
                        inter1 = inter2;
                        t = t2;
                    }
                    return inter1;
                }
            }
            else {
                cout << "Woops missed it" << endl;
                // cout << "test9" << endl;
                return vector<float>();
            }
        }
};
