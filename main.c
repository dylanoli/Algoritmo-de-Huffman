#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"list.c"

int makeMenu();
void compress();
void decompress();

int main()
{
    int option = -1;
    while (option != 0)
    {
        option = makeMenu();
    }
    
    return 0;
}

int makeMenu()
{
    int option = -1;
    printf("\n---------- Menu Principal ----------");
    printf("\n[1] Compactar Arquivo");
    printf("\n[2] Descompactar Arquivo");
    printf("\n[0] Sair");
    printf("\n\nSelecione uma opcao: ");
    scanf("%d",&option);
    system("clear"); //limpa o console no linux
    system("cls"); //limpa o console no windows

    switch (option)
    {
    case 1:
        compress();
        break;
    case 2:
        decompress();
        break;
    case 0:
        printf("\nPrograma finalizado.");
        break;
    default:
        printf("\nOpcao invalida, tente novamente.");
        break;
    }
    return option;
}

void compress()
{
    printf("\n---------- Compactar ----------");
    printf("\nDigite o diretorio do arquivo: ");
    fflush(stdin);
    char pathFile[200];
    scanf("%s[^\n]",pathFile);
    FILE * pFile = fopen(pathFile,"rb");
    if (pFile == NULL)
    {
        fclose(pFile);
        printf("Arquivo nao encontrado!");
    }
    else
    {
        char c;
        List * list = startList();
        List * listStr = startList();
        NodeList * nodeList = startNodeList();
        while (fread(&c,1,1,pFile)>0)
        {
            List * element = searchByWord(list,c);
            if (element == NULL)//caso nao encontre elemento
            {
                addEndChar(list,c);
            }
            else
            {
                element->dado.qtd++;
            }
            addEndChar(listStr,c);
            printf("%c   ",c);
        }
        fclose(pFile);
        if (list->listProx != NULL)//verifica se o arquivo está vazio
        {
            printf("\nCompactando arquivo...");
            quicksort(list, 1, list->id-1);//organiza a lista encadeada
            
            showList(*list);

            while (list->listProx != NULL)
            {
                Node * node = transformNode(list->listProx->dado);
                removeStart(list);
                addNodeStart(nodeList, *node);
            }
            while (nodeList->listProx != NULL && nodeList->listProx->listProx != NULL)
            {
                NodeList * nodeListAux = startNodeList();
                while (nodeList->listProx != NULL && nodeList->listProx->listProx != NULL)
                {
                    Node * nodeRight = &nodeList->listAnte->node;
                    removeNodeLast(nodeList);
                    Node * nodeLeft = &nodeList->listAnte->node;
                    removeNodeLast(nodeList);
                    Node * node = startNodeWithElements(*nodeRight,*nodeLeft);       
                    addNodeStart(nodeListAux,*node);
                }
                if (nodeList->listProx != NULL)
                {
                    addNodeStart(nodeListAux,(nodeList->listProx->node));
                    removeNodeLast(nodeList);
                }
                while (nodeListAux->listProx != NULL)
                {
                    addNodeStart(nodeList,nodeListAux->listAnte->node);
                    removeNodeLast(nodeListAux);  
                }
            }
            Node * node = &nodeList->listProx->node;
            int lenght = lengthNodes(node);
            Table* table = (Table*)calloc(lenght,sizeof(Table));
            buildTable(table,lenght,node);

            showTable(table,lenght);

            List * strRef = listStr;
            char rest = 1;
            strcat(pathFile,".cp");
            FILE * pFileFinal = fopen(pathFile,"wb");
            char charRead;
            char flagComplete = 0;
            //---------------Gravando Arquivo--------------
            //Formato flagComplete + lenght + table + arquivoCompactado
            fprintf(pFileFinal,"%c", flagComplete);
            fwrite((const void*) & lenght,sizeof(int),1,pFileFinal);//Gravando lenght
            int i;
            for ( i = 0; i < lenght; i++) //Gravando table
            {
                fprintf(pFileFinal,"%c", table[i].word);
                fprintf(pFileFinal,"%c", table[i].code);
            }
            do//Gravando Arquivo Compactado
            {
                strRef = buildCharTable(table,strRef, &rest, &charRead,&flagComplete);
                fprintf(pFileFinal,"%c", charRead);
            }
            while (strRef != NULL || rest != 1);
            fseek(pFileFinal, 0, SEEK_SET);
            fprintf(pFileFinal,"%c", flagComplete);
            fclose(pFileFinal);
            printf("\nConcluido!");
        }
        else
        {
            printf("\nO arquivo esta vazio!");
        }
    }  
}

void decompress()
{
    printf("\n---------- Descompactar ----------");
    printf("\nDigite o diretorio do arquivo: ");
    fflush(stdin);
    char pathFile[200];
    scanf("%s[^\n]",pathFile);
    int lengthPath = strlen(pathFile);
    FILE * pFile = fopen(pathFile,"rb");
    if (pFile == NULL)
    {
        fclose(pFile);
        printf("Arquivo nao encontrado!");
    }
    else
    {
        
        printf("\nDesompactando arquivo...");
        char flagComplete;
        int lengthTable = 0;
        fread(&flagComplete,1,1,pFile);
        fread(&lengthTable,sizeof(int),1,pFile);
        Table* table = (Table*)calloc(lengthTable,sizeof(Table));
        int i;
        for (i = 0; i < lengthTable; i++)
        {
            fread(&table[i].word,1,1,pFile);
            fread(&table[i].code,1,1,pFile);
        }
        pathFile[lengthPath-3] = '\0';
        char charBase;
        char rest = 1;
        int ref = 0;
        int find = 0;
        int flagFIM = 0;
        FILE * pFileFinal = fopen(pathFile,"wb");
        while (fread(&charBase,1,1,pFile)>0 && flagFIM == 0)
        {
            char trash;
            if (fread(&trash,1,1,pFile)==0)
            {
                flagFIM = 1;
            }
            fseek(pFile, -1, SEEK_CUR);
            while (ref != 8)
            {
                char result = searchTable(charBase,table,lengthTable,&ref, &find, &rest, flagFIM, flagComplete);
                if (find == 1)
                {
                    printf("%c   ",result);
                    fprintf(pFileFinal,"%c", result);
                }
            }
            ref = 0;
        }
        fclose(pFile);
        fclose(pFileFinal);
        printf("\nConcluido!");
    }
}