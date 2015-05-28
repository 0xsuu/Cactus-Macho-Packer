
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node
{
    struct node *parentNode;
    struct node *rightNode;
    struct node *leftNode;
    int key;
    int value;
};

void insertionSort(int *a, int *b)
{
    int i, j, index, indexb;
    for (i = 1; i < 16; ++i)
    {
        index = a[i];
        indexb = b[i];
        for (j = i; j > 0 && a[j-1] > index; j--)
        {
            a[j] = a[j-1];
            b[j] = b[j-1];
        }
        
        a[j] = index;
        b[j] = indexb;
    }
}

int getSmallestNode(int *data, int *keys)
{
    for (int i = 0; i < 16; i++)
    {
        if (data[i] != 0)
        {
            return i;
        }
    }
    return -1;
}

int getSecondSmallestNode(int *data, int *keys)
{
    int first = 0;
    for (int i = 0; i < 16; i++)
    {
        if (first)
        {
            return i;
        }
        if (data[i] != 0)
        {
            first = 1;
        }
    }
    return -1;
}

struct node findRoot(struct node *jointNodes)
{
    for (int i = 15; i >= 0; i--)
    {
        if (jointNodes[i].leftNode != NULL && jointNodes[i].parentNode == NULL)
        {
            return jointNodes[i];
        }
    }
    return jointNodes[0];
}

void traverseTree(struct node someNode, int *encodeMap, int route)
{
    if (someNode.key != 16)
    {
        encodeMap[someNode.key] = route;
        return;
    }
    else
    {
        traverseTree(*someNode.leftNode, encodeMap, route<<1);
        traverseTree(*someNode.rightNode, encodeMap, (route<<1)+1);
        return;
    }
}

int getBinCount(int num)
{
    int ret=0;
    while ((num >> 1) != 0)
    {
        ret++;
        num = num >> 1;
    }
    return ret+1;
}

float getEncodeMap(int *encodeMap, char *filePath)
{
    FILE *binary = fopen(filePath, "rb");
    
    //Find file length
    fseek(binary, 0, SEEK_END);
    long fileLen = ftell(binary);
    rewind(binary);
    
    printf("Original File length: %ld bytes\n", fileLen);
    
    //Analyse and get a Huffman Tree
    int *counter;
    counter = malloc(16*sizeof(int));
    
    for (int i = 0; i<16; i++)
    {
        counter[i] = 0;
    }
    
    signed short buffer;
    for (int i = 1; i <= fileLen; i+=1)
    {
        fread(&buffer, 1, 1, binary);
        fseek(binary, i, 0);
        //printf("%x\n",buffer);
        //counter[buffer] ++;
        counter[buffer >> 4] ++;
        counter[buffer & 15] ++;
    }
    
    int i;
    
    
    int sum = 0;
    for (i = 0; i < 16; i++)
    {
        sum += counter[i];
        //printf("%x: %d\n",i,counter[i]);
    }
    printf("sum: %d half-bytes\n", sum);
    
    //Build characters and nodes
    struct node *keyNodes;
    keyNodes = malloc(16*sizeof(struct node));
    struct node *jointNodes;
    jointNodes = malloc(16*sizeof(struct node));
    
    int *characters;
    characters = malloc(16*sizeof(int));
    for (i = 0; i<16; i++)
    {
        characters[i] = i;
        keyNodes[i].key = i;
        keyNodes[i].value = counter[i];
        keyNodes[i].leftNode = NULL;
        keyNodes[i].rightNode = NULL;
        keyNodes[i].parentNode = NULL;
        jointNodes[i].key = 16;
        jointNodes[i].value = 0;
        jointNodes[i].leftNode = NULL;
        jointNodes[i].rightNode = NULL;
        jointNodes[i].parentNode = NULL;
    }
    
    //Sort
    insertionSort(counter, characters);
    
    for (i = 0; i < 16; i++)
    {
        sum += counter[i];
        printf("%x: %d\n",characters[i],counter[i]);
    }
    
    int solution = 0;
    
    int smallest = getSmallestNode(counter, characters);
    int secondSmallest = getSecondSmallestNode(counter, characters);
    
    jointNodes[0].value = counter[smallest] + counter[secondSmallest];
    jointNodes[0].leftNode = &keyNodes[characters[smallest]];
    jointNodes[0].rightNode = &keyNodes[characters[secondSmallest]];
    
    keyNodes[smallest].parentNode = &jointNodes[0];
    keyNodes[secondSmallest].parentNode = &jointNodes[0];
    
    counter[smallest] = 0;
    counter[secondSmallest] = 0;
    
    int currentJointNode = 0;
    while (getSecondSmallestNode(counter, characters) != -1)
    {
        int smallest = getSmallestNode(counter, characters);
        int secondSmallest = getSecondSmallestNode(counter, characters);
        if (jointNodes[currentJointNode].value > counter[smallest] + counter[secondSmallest])
        {
            //printf("Smallest: %x  Second: %x\n",characters[smallest], characters[secondSmallest]);
            jointNodes[currentJointNode+1].value = counter[smallest] + counter[secondSmallest];
            jointNodes[currentJointNode+1].leftNode = &keyNodes[characters[smallest]];
            jointNodes[currentJointNode+1].rightNode = &keyNodes[characters[secondSmallest]];
            
            keyNodes[characters[smallest]].parentNode = &jointNodes[currentJointNode+1];
            keyNodes[characters[secondSmallest]].parentNode = &jointNodes[currentJointNode+1];
            
            jointNodes[currentJointNode].parentNode = &jointNodes[currentJointNode+2];
            jointNodes[currentJointNode+1].parentNode = &jointNodes[currentJointNode+2];
            
            jointNodes[currentJointNode+2].value = jointNodes[currentJointNode].value + jointNodes[currentJointNode+1].value;
            jointNodes[currentJointNode+2].leftNode = &jointNodes[currentJointNode];
            jointNodes[currentJointNode+2].rightNode = &jointNodes[currentJointNode+1];
            
            counter[smallest] = 0;
            counter[secondSmallest] = 0;
            
            currentJointNode += 2;
        }
        else
        {
            //printf("Smallest: %x\n",characters[smallest]);
            jointNodes[currentJointNode+1].value = counter[smallest] + jointNodes[currentJointNode].value;
            jointNodes[currentJointNode+1].leftNode = &keyNodes[characters[smallest]];
            jointNodes[currentJointNode+1].rightNode = &jointNodes[currentJointNode];
            
            jointNodes[currentJointNode].parentNode = &jointNodes[currentJointNode+1];
            keyNodes[characters[smallest]].parentNode = &jointNodes[currentJointNode+1];
            
            counter[smallest] = 0;
            
            currentJointNode ++;
        }
        
    }
    
    jointNodes[currentJointNode+1].value = jointNodes[currentJointNode].value + counter[15];
    jointNodes[currentJointNode+1].leftNode = &keyNodes[characters[15]];
    jointNodes[currentJointNode+1].rightNode = &jointNodes[currentJointNode];
    
    jointNodes[currentJointNode].parentNode = &jointNodes[currentJointNode+1];
    keyNodes[characters[15]].parentNode = &jointNodes[currentJointNode+1];
    
    counter[15] = 0;

    //Traverse tree to generate map
    traverseTree(jointNodes[currentJointNode+1], encodeMap, 0);

    for (int i = 0; i < 16; i++)
    {
        solution += getBinCount(encodeMap[i])*keyNodes[i].value;
    }
    printf("solu: %d bits\n",solution);
    
    return 100.0*(fileLen*8.0-solution*1.0)/(fileLen*8.0);
}

int main(int argc, char *argv[])
{
    int encodeMap[16] = {0}; //Empty encodeMap
    float ratio = getEncodeMap(encodeMap, argv[1]);
    
    int i = 0;
    for (i = 0; i < 16; i++)
    {
        printf("%x: %x\n", i, encodeMap[i]);
    }
    
    printf("Compressed ratio: %f%%\n", ratio);
    
    return 0;
}
