//
// Created by Caroline Cullen on 2019-05-21.
//

#include "BoundingVolumeHierarchy.h"

void BoundingVolumeHierarchy::init(std::vector<GameObject*> objs)
{
    std::vector<GameObject*> sortedByX = objs;
    std::sort(sortedByX.begin(), sortedByX.end());
    glm::vec3 min_hld = glm::vec3(sortedByX[0]->bs->midpt.x + sortedByX[0]->bs->radius, sortedByX[0]->bs->midpt.y + sortedByX[0]->bs->radius, sortedByX[0]->bs->midpt.z + sortedByX[0]->bs->radius);
    glm::vec3 max_hld = glm::vec3(sortedByX[sortedByX.size()-1]->bs->midpt.x + sortedByX[sortedByX.size()-1]->bs->radius, sortedByX[sortedByX.size()-1]->bs->midpt.y + sortedByX[sortedByX.size()-1]->bs->radius, sortedByX[sortedByX.size()-1]->bs->midpt.z + sortedByX[sortedByX.size()-1]->bs->radius);
    root = new BoundingBox(min_hld, max_hld);

    createBVH(root, objs);
    //printBVH(root);
}

void BoundingVolumeHierarchy::printBVH(BoundingBox* cur)
{
    for (int i = 0; i < cur->objects_contained.size(); i++)
    {
        if(dynamic_cast<Strawberry*>(cur->objects_contained[i]) != nullptr)
        {
            auto hld = dynamic_cast<Strawberry*>(cur->objects_contained[i]);
            std::cout << hld->color << std::endl;
        }
        else
        {
            std::cout << cur->objects_contained[i]->color << std::endl;
        }
    }

    if(cur->right_box != nullptr)
        printBVH(cur->right_box);
    if(cur->left_box != nullptr)
        printBVH(cur->left_box);
}

bool BoundingVolumeHierarchy::checkForCollision(std::vector<GameObject*> &gameobjs, BoundingBox* cur, BoundingSphere* bs, bool &collided)
{
    if(cur->right_box != nullptr && cur->left_box != nullptr)
    {
        if(cur->isCollided(bs))
        {
            checkForCollision(gameobjs, cur->right_box, bs, collided);
            checkForCollision(gameobjs, cur->left_box, bs, collided);
        }
    }

    GameObject* hold = checkAgainstShapes(cur, bs, collided);
    for (int i = 0; i < gameobjs.size(); i++)
    {
        if((gameobjs[i] == hold) && (dynamic_cast<Strawberry*>(gameobjs[i]) != nullptr))
        {
            auto hld = dynamic_cast<Strawberry*>(gameobjs[i]);
            hld->collect();
        }
    }

    if(hold!=nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }

}

GameObject* BoundingVolumeHierarchy::checkAgainstShapes(BoundingBox* cur, BoundingSphere* bs, bool& collided)
{

    for(int i = 0; i < cur->objects_contained.size(); i++)
    {
        if (cur->objects_contained[i]->isCollided(bs))
        {
            collided = true;
            return cur->objects_contained[i];
        }
    }
    return nullptr;
}

void BoundingVolumeHierarchy::createBVH(BoundingBox* bb, std::vector<GameObject*> objs)
{
//    std::cout << "Creating volume hierarchy recursive call." << std::endl;
    // meaning there are at least 5 objects in the objects vector
    if((objs.size()-1) > 3)
    {
//        std::cout << "obj size with more than 5 object: " << objs.size() << std::endl;
        float xaxis = bb->bbmax.x - bb->bbmin.x;
        float zaxis = bb->bbmax.z - bb->bbmin.z;
        std::vector<GameObject*> sortedBy = objs;
        int split_index = 0;

        if(xaxis > zaxis)
        {
            std::sort(sortedBy.begin(),sortedBy.end());
        }
        else
        {
            std::vector<GameObject*> sortedBy = objs;
            for(int i = 0; i < objs.size(); i++)
            {
                objs[i]->comparator_value = 1;
            }
            std::sort(sortedBy.begin(),sortedBy.end());
        }

        if(sortedBy.size()%2 == 0)
        {
            split_index = sortedBy.size()/2;
        }
        else
        {
            split_index = (sortedBy.size()+1)/2;
        }

        glm::vec3 min_hld = glm::vec3(sortedBy[0]->bs->midpt.x - sortedBy[0]->bs->radius, sortedBy[0]->bs->midpt.y - sortedBy[0]->bs->radius, sortedBy[0]->bs->midpt.z - sortedBy[0]->bs->radius);
        glm::vec3 max_hld = glm::vec3(sortedBy[split_index-1]->bs->midpt.x + sortedBy[split_index-1]->bs->radius, sortedBy[split_index-1]->bs->midpt.y + sortedBy[split_index-1]->bs->radius, sortedBy[split_index-1]->bs->midpt.z + sortedBy[split_index-1]->bs->radius);
        bb->right_box = new BoundingBox(min_hld, max_hld);
        createBVH(bb->right_box, splitArray(sortedBy, 0, split_index));

        min_hld = glm::vec3(sortedBy[split_index]->bs->midpt.x - sortedBy[split_index]->bs->radius, sortedBy[split_index]->bs->midpt.y - sortedBy[split_index]->bs->radius, sortedBy[split_index]->bs->midpt.z - sortedBy[split_index]->bs->radius);
        max_hld = glm::vec3(sortedBy[(sortedBy.size()-1)]->bs->midpt.x + sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.y + sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.z + sortedBy[(sortedBy.size()-1)]->bs->radius);
        bb->left_box = new BoundingBox(min_hld, max_hld);
        createBVH(bb->left_box, splitArray(sortedBy, split_index, sortedBy.size()));
    }
    else
    {
        // when there are at least 4 objects left
        // ie 4 or 3 or 1 or none but for our game it will be 4 or 3
        float xaxis = bb->bbmax.x - bb->bbmin.x;
        float zaxis = bb->bbmax.z - bb->bbmin.z;
        std::vector<GameObject*> sortedBy = objs;

//        std::cout << "obj size with less than 5 object: " << objs.size() << std::endl;
        int split_index = 0;
        if(xaxis > zaxis)
        {
            std::sort(sortedBy.begin(),sortedBy.end());
        }
        else
        {
            std::vector<GameObject*> sortedBy = objs;
            for(int i = 0; i < objs.size(); i++)
            {
                objs[i]->comparator_value = 1;
            }
            std::sort(sortedBy.begin(),sortedBy.end());
        }

        split_index = 1;

        glm::vec3 min_hld = glm::vec3(sortedBy[0]->bs->midpt.x - sortedBy[0]->bs->radius, sortedBy[0]->bs->midpt.y - sortedBy[0]->bs->radius, sortedBy[0]->bs->midpt.z - sortedBy[0]->bs->radius);
        glm::vec3 max_hld = glm::vec3(sortedBy[split_index]->bs->midpt.x + sortedBy[split_index]->bs->radius, sortedBy[split_index]->bs->midpt.y + sortedBy[split_index]->bs->radius, sortedBy[split_index]->bs->midpt.z + sortedBy[split_index]->bs->radius);
        bb->right_box = new BoundingBox(min_hld, max_hld);
        bb->right_box->objects_contained = splitArray(sortedBy, 0, split_index+1);

        if((objs.size()-1) == 3)
        {
            // this means there are 4 objects in object vector
//            std::cout << "actual split" << sortedBy.size() << std::endl;
            min_hld = glm::vec3(sortedBy[split_index+1]->bs->midpt.x - sortedBy[split_index+1]->bs->radius, sortedBy[split_index+1]->bs->midpt.y - sortedBy[split_index+1]->bs->radius, sortedBy[split_index+1]->bs->midpt.z - sortedBy[split_index+1]->bs->radius);
            max_hld = glm::vec3(sortedBy[(sortedBy.size()-1)]->bs->midpt.x + sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.y + sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.z + sortedBy[(sortedBy.size()-1)]->bs->radius);
            bb->left_box = new BoundingBox(min_hld, max_hld);
            bb->left_box->objects_contained = splitArray(sortedBy, split_index, sortedBy.size()-1);
        }
        else
        {
            // this means there are 3 object in object vector
//            std::cout << "pseudo split: " << sortedBy.size() << std::endl;
            min_hld = glm::vec3(sortedBy[(sortedBy.size()-1)]->bs->midpt.x - sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.y - sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.z - sortedBy[(sortedBy.size()-1)]->bs->radius);
            max_hld = glm::vec3(sortedBy[(sortedBy.size()-1)]->bs->midpt.x + sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.y + sortedBy[(sortedBy.size()-1)]->bs->radius, sortedBy[(sortedBy.size()-1)]->bs->midpt.z + sortedBy[(sortedBy.size()-1)]->bs->radius);
            bb->left_box = new BoundingBox(min_hld, max_hld);
            std::vector<GameObject*> arr;
            arr.push_back(sortedBy[sortedBy.size()-1]);
            bb->left_box->objects_contained = arr;
        }

        return;
    }

}

std::vector<GameObject*> BoundingVolumeHierarchy::splitArray(std::vector<GameObject*> og_array, int index_begin, int index_end)
{
    std::vector<GameObject*> arr;
    for(int i = index_begin; i < index_end; i++)
        arr.push_back(og_array[i]);

    return arr;
}