# CTU parallel algorithms course 
[link to the page](https://cw.fel.cvut.cz/wiki/courses/pag/start)

### I took it just for fun and practiced my C++ and parallel algorithm skills
- course is interesting
- unfortunately, I could not attend lectures, because of 'three-dimensional computer vision' course at the same time (and the last one was more important for me)
- seminars are Ok, but
1) I did not like the code style of those guys. `using namespace` is a terrible practice, and give it to students... also, they use something like `int n = vector.size()`... Ok, maybe they don't know about the AAA principle (almost always auto), but at least, use `size_t` for vector size variable!
2) I did not like their cmake's without sanitisers, PVS etc, so I used mine
3) Everything is in a rush. Sometimes during lab works teacher complete the task, show the "correct" solution, but with data race or memory leak... so you should be careful and check it on different input data
