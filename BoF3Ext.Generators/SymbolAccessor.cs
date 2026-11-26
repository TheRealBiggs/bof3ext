using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Microsoft.CodeAnalysis;

using Scriban;
using Scriban.Parsing;
using Scriban.Runtime;

namespace BoF3Ext.Generators;


sealed class SymbolAccessor(Type type) : IObjectAccessor {
	readonly Dictionary<string, PropertyInfo> properties = type.GetInterfaces()
			.Where(x => typeof(ISymbol).IsAssignableFrom(x))
			.SelectMany(x => x.GetProperties())
			.Where(x => x.CanRead && x.GetIndexParameters().Length == 0)
			.GroupBy(x => x.Name)
			.ToDictionary(x => x.Key, x => x.First());


	public bool HasIndexer => false;
	public Type? IndexType => null;


	public int GetMemberCount(TemplateContext context, SourceSpan span, object target)
		=> properties.Count;

	public IEnumerable<string> GetMembers(TemplateContext context, SourceSpan span, object target)
		=> properties.Keys;

	public bool HasMember(TemplateContext context, SourceSpan span, object target, string member)
		=> properties.ContainsKey(member);

	public bool TryGetValue(TemplateContext context, SourceSpan span, object target, string member, out object? value) {
		if (properties.TryGetValue(member, out var property)) {
			value = property.GetValue(target);

			return true;
		}

		value = null;

		return false;
	}

	public bool TrySetValue(TemplateContext context, SourceSpan span, object target, string member, object? value)
		=> false;

	public bool TryGetItem(TemplateContext context, SourceSpan span, object target, object index, out object? value) {
		value = null;

		return false;
	}

	public bool TrySetItem(TemplateContext context, SourceSpan span, object target, object index, object? value)
		=> false;
}


sealed class SymbolTemplateContext : TemplateContext {
	protected override IObjectAccessor? GetMemberAccessorImpl(object target)
		=> target is ISymbol
			? new SymbolAccessor(target.GetType())
			: base.GetMemberAccessorImpl(target);
}