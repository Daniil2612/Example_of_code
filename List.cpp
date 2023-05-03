#include <iostream>
using namespace std;

struct ListNode {
    int value;
    ListNode* next; 
};
void DelNext( ListNode* head ) {
    ListNode *curr=head, *next=NULL;
    while (curr!=NULL) {
    	next=curr->next;
    	delete curr;
    	curr=next;
    }
}

ListNode* Process(ListNode* head) {
     ListNode *q, *out, *p , *pr;
    out = NULL; 
 while (head !=NULL){
        q = head; 
        head = head->next;
    	for ( p=out,pr=NULL; p!=NULL && q->value>p->value; pr=p,p=p->next);
    	if (pr==NULL){
    	    q->next=out;
    	    out=q; 
    	}
        else{ 
            q->next=p; 
            pr->next=q; 
        }
	}
	head=out;
	ListNode *h, *g=head;
	int i=1;
    while(i){
        i=0;
        g=head;
        while(g->next!=nullptr){
            h=g->next;
            if(g->value==h->value){
                g->next=h->next;
                delete h;
                i=1;
            }
            else{
            	g=g->next;
			}
        }
    }
    return out;
}

int main(void) {
    ListNode *prev=NULL,  *Now, *New;
    int i, k, l, elem;
    std::cout<<"Введите число элементов списка"<<std::endl;
    std::cin>>k;
    std::cout<<"Введите элементы списка"<<std::endl;
    for (i = 0; i <  k; i++)  {
        std::cin >> elem;
        New = new ListNode;
        New->value = elem;
        New->next = nullptr;
        if (prev == nullptr) {
        	prev = New;
  		}
        else {
            Now = prev;
            while (Now->next != nullptr) {
                Now = Now->next;
            }
            Now->next = New;
        }
    }
    prev=Process( prev);
     while (prev != nullptr) {
        cout << prev->value << " ";
        prev = prev->next;
    }  
    DelNext(prev);
}