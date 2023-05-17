import numpy as np


class database():
    def __init__(self,Window=3,SampleRate=30):
        self.SampleRate = SampleRate
        self.Window = Window
        self.Length = self.SampleRate * self.Window
        self.Pointer = 0
        self.Data = np.zeros((self.Length, 5))
        self.startAna=False

    def add(self,data):
        self.Data[self.Pointer,:]=np.array(data)
        self.movePointer()

    #overwrite data at current pointer
    def overwrite(self,data):
        self.Data[self.Pointer, :] = np.array(data)

    def movePointer(self):
        self.Pointer+=1
        if self.Pointer>=self.Length:
            self.Pointer=0
            self.startAna=True

    def top(self):
        toppoint=0
        if self.Pointer==0:
            toppoint = self.Length-1
        else:
            toppoint=self.Pointer-1

        return self.Data[toppoint,:]

    def get(self):
        return self.Data

class shoesDatabase():
    def __init__(self,Window=3,SampleRate=30):
        self.leftdatabase=database(Window,SampleRate)
        self.rightdatabase=database(Window,SampleRate)
        self.anaPointer=0#-1 left, 1 right
        self.d=0.46
        self.h=1

    def add(self,device,data):
        if device=="left":
            if self.anaPointer==-1:
                self.leftdatabase.overwrite(data)
            elif self.anaPointer==0:
                self.leftdatabase.add(data)
                self.anaPointer=-1
            elif self.anaPointer==1:
                self.leftdatabase.add(data)
                self.anaPointer=0
                self.anaData()
        else:
            if self.anaPointer == 1:
                self.rightdatabase.overwrite(data)
            elif self.anaPointer == 0:
                self.rightdatabase.add(data)
                self.anaPointer = 1
            elif self.anaPointer == -1:
                self.rightdatabase.add(data)
                self.anaPointer = 0
                self.anaData()


    def anaData(self):
        self.instantDataana()
        if self.rightdatabase.startAna and self.leftdatabase.startAna:
            self.meanDataana()

    def meanDataana(self):
        pass

    def instantDataana(self):
        p1=np.sum(self.leftdatabase.top())
        p2=np.sum(self.rightdatabase.top())
        if p1<0:
            p1=0
        if p2<0:
            p2=0
        if (p1<0.06 or p2<0.06) and (abs(p1-p2)<0.06):
            xb=self.d / 2
            d = 0
        else:
            tanb = (p1 + p2) * self.h / (self.d * p1)
            xb = self.h / tanb
            d = self.d / 2 - xb
        print("Used left:",self.leftdatabase.top(),"Used right:",self.rightdatabase.top())
        print("xb:"+str(xb),"d="+str(d))




if __name__=="__main__":
    local=shoesDatabase()
    for i in range(150):
        local.add("left",[2,3,4,1,2])
        local.add("right",[2,3,4,1,2])
