
/* Function: checkArgumentList */
Type checkArgumentList(vector<Type> args, Symbol* s){
	int i;
	vector<Type> params;
	params=*(s->type().parameters());
	for(i=0;i<args.size();i++){
		if(!args[i].isSimple()){
			return Type();
		}
	}
	return Type();
}

Type checkEqualityExpression(Type left, Type right, string op){
	if(left.isError() || right.isError())
		return Type();
	if(!left.isCompatible(right)){
		report("nope",op);
		return Type();
	}
	return Type("int");
}

Type checkAdditiveExpression(Type left, Type right, string op){
	if(left.isError() || right.isError())
		return Type();
	if(!left.isCompatible(right)){
		report("nope",op);
		return Type();
	}
	//If op == +
	//if left=type int && right=type pointer to T RETURN TYPE IS PTR TO T
	
	//if op == -
	//if left
}

Type checkRelationalExpression(Type left, Type right, string op){
	if(left.isError() || right.isError())
		return Type();
	if(!left.isCompatible(right)){
		report("nope",op);
		return Type();
	}
	return Type("int");
}
Type checkMultiplicativeExpression(Type left, Type right, string op){
	if(left.isError() || right.isError())
		return Type();
	if(!left.isCompatible(right)){
